import boto3
import json
import struct

def get_data_bounds(data):
    # Return a dict of two lists, containing max and min for each column.
    # Assumes labels are being stored right now.
    max_in_col = {}
    min_in_col = {}
    for r in data:
        for idx, v in r:
            if idx not in max_in_col:
                max_in_col[idx] = v
            if idx not in min_in_col:
                min_in_col[idx] = v
            if v > max_in_col[idx]:
                max_in_col[idx] = v
            if v < min_in_col[idx]:
                min_in_col[idx] = v
    return {
        "max": max_in_col,
        "min": min_in_col
    }

def get_data_from_s3(client, src_bucket, src_object, keep_label=False):
    # Return a 2D list, where each element is a row of the dataset.
    print("Getting bytes from boto3")
    b = client.get_object(Bucket=src_bucket, Key=src_object)["Body"].read()
    print("Got {0} bytes".format(len(b)))
    data = []
    labels = []
    c = []
    idx = None
    label_bytes = None
    num_values = None
    seen = 0
    print("Set local variables")
    for i in range(8, len(b), 4):
        if label_bytes is None:
            label_bytes = b[i:i+4]
            if keep_label:
                labels.append(label_bytes)
            continue
        if num_values is None:
            num_values = struct.unpack("i", b[i:i+4])[0]
            continue
        if seen % 2 == 0:
            idx = struct.unpack("i", b[i:i+4])[0]
        else:
            c.append((idx, struct.unpack("f", b[i:i+4])[0]))
        seen += 1
        if seen == num_values * 2:
            data.append(c)
            c = []
            label_bytes = None
            num_values = None
            seen = 0
    if keep_label:
        return data, labels
    return data

def put_bounds_in_s3(client, bounds, dest_bucket, dest_object):
    # Add the dictionary of bounds to an S3 bucket. 
    s = json.dumps(bounds)
    client.put_object(Bucket=dest_bucket, Key=dest_object, Body=s)

def get_global_bounds(client, bucket, src_object):
    # Get the bounds across all objects.
    b = client.get_object(Bucket=bucket, Key=src_object + "_final_bounds")["Body"].read()
    print("Global bounds are {0} bytes".format(len(b)))
    return json.loads(b.decode("utf-8"))

def scale_data(data, g, min_v, max_v):
    for r in data:
        for j in range(len(r)):
            idx_t, v = r[j]
            idx = str(idx_t)
            s = (max_v + min_v) / 2.0
            if g["min"][idx] != g["max"][idx]:
                s = (v - g["min"][idx]) / (g["max"][idx] - g["min"][idx]) * (max_v - min_v) + min_v
            r[j] = (idx, s)
    return data

def serialize_data(data, labels):
    lines = []
    num_bytes = 0
    for idx in range(len(data)):
        c = []
        c.append(labels[idx])
        c.append(struct.pack("i", len(data[idx])))
        for idx2, v2 in data[idx]:
            c.append(struct.pack("i", int(idx2)))
            c.append(struct.pack("f", float(v2)))
        lines.append(b"".join(c))
        num_bytes += len(lines[-1])
    return struct.pack("i", num_bytes + 8) + struct.pack("i", len(labels)) + b"".join(lines)

def handler(event, context):
    # Either calculates the local bounds, or scales data and puts the new data in
    # {src_object}_scaled.
    print(event["s3_bucket_input"], event["s3_key"])
    client = boto3.client("s3")
    if event["action"] == "LOCAL_BOUNDS":
        print("Getting data from S3...")
        d = get_data_from_s3(client, event["s3_bucket_input"], event["s3_key"])
        print("Getting local data bounds...")
        b = get_data_bounds(d)
        print("Putting bounds in S3...")
        put_bounds_in_s3(client, b, event["s3_bucket_input"], event["s3_key"] + "_bounds")
    elif event["action"] == "LOCAL_SCALE":
        assert "s3_bucket_output" in event, "Must specify output bucket."
        print("Getting data from S3...")
        d = get_data_from_s3(client, event["s3_bucket_input"], event["s3_key"], keep_label=True)
        print("Getting global bounds...")
        b = get_global_bounds(client, event["s3_bucket_input"], event["s3_key"])
        print("Scaling data...")
        scaled = scale_data(d[0], b, event["min_v"], event["max_v"])
        print("Serializing...")
        serialized = serialize_data(scaled, d[1])
        print("Putting in S3...")
        client.put_object(Bucket=event["s3_bucket_output"], Key=event["s3_key"], Body=serialized)
    return []