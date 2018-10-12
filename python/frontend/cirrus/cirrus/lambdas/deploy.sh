rm bundle.zip
zip -9r bundle.zip handler.py minmaxhandler.py ../utils.py normalhandler.py redis/ rediscluster/ toml.py redis.toml featurehashinghandler.py
aws lambda update-function-code --function-name neel_lambda --zip-file fileb://bundle.zip&
aws logs delete-log-group --log-group-name /aws/lambda/neel_lambda
rm bundle.zip