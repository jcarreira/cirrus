"""Utilities for setting up an installation of Cirrus.

Run this using `python -m cirrus.setup`! It uses relative imports.
"""

import textwrap
import os
import sys

import boto3
import botocore.exceptions

from . import configuration
from . import automate


# The path at which boto3 expects the user's AWS credentials. Must be passed
#   through os.path.expanduser.
AWS_CREDENTIALS_PATH = "~/.aws/credentials"


# An S3 URL at which the maintainers have published a Cirrus build.
PUBLISHED_BUILD = "s3://cirrus-public-sheacirrus"


# The name to give to the worker Lambda.
LAMBDA_NAME = "cirrus_worker"


# The name to give to the server AMI.
SERVER_IMAGE_NAME = "cirrus_server_image"


def run_interactive_setup():
    """Run an interactive command-line setup process.

    If passed the flag "--instance-resources-only", only sets up the AWS
        credentials and instance resources.
    """
    configuration.config(False)["aws"] = {}

    _set_up_aws_credentials()

    _set_up_region()

    _set_up_instance_resources()

    if len(sys.argv) > 1:
        if sys.argv[1] == "--instance-resources-only":
            return

    _set_up_bucket()

    _make_lambda()

    _make_server_image()

    _save_config()

    print("")
    print("")
    print("Done.")


def _set_up_aws_credentials():
    """If the user does not already have functioning AWS credentials in place,
        prompt for AWS credentials and obtain permission to save them to
        AWS_CREDENTIALS_PATH.
    """
    # If we are authorized even without us specifying explicit credentials,
    #   then the user already has credentials set up somewhere.
    if _aws_authorized():
        return

    EXPLANATION = textwrap.dedent("""\
        Please enter the ID of one of your AWS access keys. This will enable
            Cirrus to create AWS resources on your behalf. See
            https://amzn.to/2CagUqm for how to retrieve this information.""")
    PROMPTS = ("Access key ID", "Secret access key")
    id, secret = prompt(EXPLANATION, PROMPTS, _aws_authorized)

    EXPLANATION = "May Cirrus write your AWS credentials to %s?" \
                  % AWS_CREDENTIALS_PATH
    PROMPTS = ("y/n",)
    validator = lambda c: c  in ("y", "n")
    postprocessor = lambda c: c == "y"
    can_write = prompt(EXPLANATION, PROMPTS, validator, postprocessor)

    if not can_write:
        print("Please set up your AWS credentials manually, so that they can "
              "be read by boto3.")
        return

    credentials = textwrap.dedent("""\
        [default]
        aws_access_key_id = %s
        aws_secret_access_key = %s""" % (id, secret))
    path = os.path.expanduser(AWS_CREDENTIALS_PATH)
    if not os.path.exists(os.path.dirname(path)):
        os.makedirs(os.path.dirname(path))
    with open(path, "w+") as f:
        f.write(credentials)


def _aws_authorized(id=None, secret=None):
    """Return whether the user is authorized to access AWS.

    Args:
        id (str): The user's access key ID. If omitted or None, the access key
            ID that `boto3` is configured with, if any, will be used.
        secret (str): The user's secret access key. If omitted or None, the
            secret access key that `boto3` is configured with, if any, will be
            used.

    Returns:
        bool: Whether the user is authorized.
    """
    session = boto3.session.Session(id, secret)
    ec2 = session.client("ec2", "us-west-1")
    try:
        # We're doing this to test out the account information. The only way to
        #   check it is to try using it to sign a request.
        ec2.describe_regions()
    except botocore.exceptions.ClientError as e:
        if e.args[0].startswith("An error occurred (AuthFailure)"):
            return False
        else:
            raise e
    except botocore.exceptions.NoCredentialsError:
        return False
    else:
        return True


def _set_up_region():
    """Prompt the user for their preferred AWS region and add it to the
        configuration.
    """
    EXPLANATION = "What AWS region do you want Cirrus to use?"
    PROMPTS = ("Region",)
    regions = boto3.session.Session().get_available_regions("lambda")
    validator = lambda region: region in regions

    region = prompt(EXPLANATION, PROMPTS, validator)

    configuration.config(False)["aws"]["region"] = region

    # Refresh cached AWS clients, so that clients are bound to the updated
    #   region.
    automate.clients.clear_cache()


def _make_lambda():
    """Make the worker Lambda, prompting the user for permission.
    """
    explanation = ("Can we create a Lambda function named '%s' in your AWS" 
                   " account?") % LAMBDA_NAME
    PROMPTS = ("y/n",)
    validator = lambda c: c in ("y", "n")
    postprocess = lambda c: c == "y"
    if not prompt(explanation, PROMPTS, validator, postprocess):
        print("Exiting. Cirrus will not be usable. Re-run the setup script to "
              "complete setup.")
        return

    explanation = "How many concurrent executions, at maximum, should the " \
                  "Lambda function be limited to? Your AWS account must have " \
                  "at least this many unreserved concurrent executions " \
                  "available in the %s region." \
                  % configuration.config(False)["aws"]["region"]
    PROMPTS = ("Executions",)
    # TODO: Actually check that the chosen number is valid. It should be less
    #   than the account's limit - 100.
    def validator(s):
        try:
            return int(s) > 0
        except ValueError:
            return False
    postprocess = lambda s: int(s)
    concurrency = prompt(explanation, PROMPTS, validator, postprocess)

    print("Creating the Lambda function. This may take a minute.")
    package_url = PUBLISHED_BUILD + "/lambda_package"
    automate.make_lambda(LAMBDA_NAME, package_url, concurrency)


def _make_server_image():
    """Make the server image, prompting the user for permission.
    """
    explanation = ("Can we create an AMI named '%s' in your AWS account?"
                   % SERVER_IMAGE_NAME)
    PROMPTS = ("y/n",)
    validator = lambda c: c in ("y", "n")
    postprocess = lambda c: c == "y"
    if not prompt(explanation, PROMPTS, validator, postprocess):
        print("Exiting. Cirrus will not be usable. Re-run the setup script to "
              "complete setup.")
        return

    print("Creating the server image. This may take a few minutes. This will "
          "involve launching an EC2 instance. If any error should occur, "
          "please use the AWS console to manually terminate the instance and "
          "avoid ongoing charges for it.")
    executables_url = PUBLISHED_BUILD + "/executables"
    automate.make_server_image(SERVER_IMAGE_NAME, executables_url)


def _set_up_instance_resources():
    """Set up resources that are needed by `automate.Instance`.
    """
    explanation = ("Can we create an IAM role named '%s' in your AWS account?"
                   % automate.Instance.ROLE_NAME)
    PROMPTS = ("y/n",)
    validator = lambda c: c in ("y", "n")
    postprocess = lambda c: c == "y"
    if not prompt(explanation, PROMPTS, validator, postprocess):
        print("Exiting. Cirrus will not be usable. Re-run the setup script to "
              "complete setup.")
        return
    automate.Instance.set_up_role()

    explanation = ("Can we create a key pair named '%s' in your AWS account?"
                   % automate.Instance.KEY_PAIR_NAME)
    PROMPTS = ("y/n",)
    validator = lambda c: c in ("y", "n")
    postprocess = lambda c: c == "y"
    if not prompt(explanation, PROMPTS, validator, postprocess):
        print("Exiting. Cirrus will not be usable. Re-run the setup script to "
              "complete setup.")
        return
    automate.Instance.set_up_key_pair()

    explanation = ("Can we create a security group named '%s' in your AWS "
                   "account?" % automate.Instance.SECURITY_GROUP_NAME)
    PROMPTS = ("y/n",)
    validator = lambda c: c in ("y", "n")
    postprocess = lambda c: c == "y"
    if not prompt(explanation, PROMPTS, validator, postprocess):
        print("Exiting. Cirrus will not be usable. Re-run the setup script to "
              "complete setup.")
        return
    automate.Instance.set_up_security_group()

    explanation = ("Can we create an instance profile named '%s' in your AWS "
                   "account?" % automate.Instance.INSTANCE_PROFILE_NAME)
    PROMPTS = ("y/n",)
    validator = lambda c: c in ("y", "n")
    postprocess = lambda c: c == "y"
    if not prompt(explanation, PROMPTS, validator, postprocess):
        print("Exiting. Cirrus will not be usable. Re-run the setup script to "
              "complete setup.")
        return
    automate.Instance.set_up_instance_profile()


def _set_up_bucket():
    """Set up Cirrus' bucket in the user's AWS account.
    """
    explanation = ("Can we create an S3 bucket named '%s' in your AWS account?"
                   % automate.get_bucket_name())
    PROMPTS = ("y/n",)
    validator = lambda c: c in ("y", "n")
    postprocess = lambda c: c == "y"
    if not prompt(explanation, PROMPTS, validator, postprocess):
        print("Exiting. Cirrus will not be usable. Re-run the setup script to "
              "complete setup.")
        return
    automate.set_up_bucket()


def _save_config():
    """Save the configuration.
    """
    with open(os.path.expanduser(configuration.CONFIGURATION_PATH), "w+") as f:
        configuration.config(False).write(f)


def prompt(explanation, prompts, validator=None, postprocess=None):
    """Prompt the user for some pieces of information.

    Prints the explanation. Prints each prompt in turn. Uses `validator` to
        check the user's provided values and retry if invalid. Uses
        `postprocess` to process the values before returning them.

    Args:
        explanation (str): A paragraph explanation that provides context for
            the request.
        prompts (list[str]): Prompts, one for each piece of information that is
            desired.
        validator (func[str, ...] -> bool): A function that is called with the
            user's provided values. Returns true iff the values are valid. If
            omitted or None, all values are considered valid.
        postprocess (func[str, ...] -> tuple[*]): A function that is called
            with the user's provided values if they are valid. Should return
            processed versions of them. If omitted or None, no postprocessing
            occurs.

    Returns:
        *: The return value of `postprocess` called on the user's provided
            values.
    """
    if validator is None:
        def validator(*args):
            return True

    if postprocess is None:
        def postprocess(*args):
            if len(args) == 1:
                return args[0]
            return args

    print(explanation)

    while True:
        values = [raw_input(prompt + ": ") for prompt in prompts]
        if validator(*values):
            break
        else:
            print("")
            print("Invalid.")

    print("")
    print("")
    return postprocess(*values)


if __name__ == "__main__":
    run_interactive_setup()