def config_gen(**kwargs):
    args = locals()

    valid_args = ["input_path", "input_type", "minibatch_size", "s3_size",
                  "model_type", "learning_rate", "epsilon", "normalize",
                  "limit_samples", "use_bias", "num_users", "num_items",
                  "train_set", "s3_bucket"]

    config = ""
    kwargs = args['kwargs']
    for arg in kwargs.keys():
        if arg not in valid_args:
            print("arg:", arg)
            raise Exception("Unknown arg")

        if config: # not empty
            config = config + "\n"
        config = config + "%s: %s" % (arg, kwargs[arg])

    return config

if __name__ == "__main__":
    # test this file
    config = config_gen(input_path=3, minibatch_size="100")
    print config;
    try:
        config = config_gen(unknown=1)
        print config
    except Exception as e:
        pass
