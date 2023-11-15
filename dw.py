from fastapi import FastAPI, Body
from kubernetes import client， config

config.kube_config.load_kube_config(config_file='/root/.kube/config')

app = FastAPI()

@app.get("/pods")
async def list_pods():
    """
    获取命令空间为www下所有的pod名称。

    :return: pod 名称列表。
    """

    configuration = client.Configuration()
    v1 = client.CoreV1Api()
    pods = v1.list_namespaced_pod(namespace="www")
    res = []
    for pod in pods.otems:
        res.append(pod.metadata.name)
    return res


def delete_pods(pod_names: List[str]):
    """
    删除 Pod。

    :param pod_names: Pod 的名称列表。
    :return: 删除 Pod 的结果。
    """

    configuration = client.Configuration()
    v1 = client.CoreV1Api()

        # 删除 Pod。
    try:
        for pod_name in pod_names:
            api_response = v1.delete_namespaced_pod(
                pod_name,
                "www",
                grace_period_seconds=56,
                orphan_dependents=True
            )
        return {"success": True}
    except ApiException as e:
        return {"success": False, "message": e.body["message"]}


@app.post("/restart1")
async def restart1():
    """
    删除list1中所有的pod。

    :return: 删除 Pod 的结果。
    """

    pods = await list_pods()
    list1 = []
    for pod in pods:
        if "ml1" in pod.metadata.name:
            list1.append(pod.metadata.name)

    # 删除 Pod。
    result = delete_pods(list1)

    return result


@app.post("/restart2")
async def restart2():
    """
    删除list2里面的所有接口。

    :return: 删除 Pod 的结果。
    """

    pods = await list_pods()
    list2 = []
    for pod in pods:
        if "ml2" in pod.metadata.name:
            list2.append(pod.metadata.name)

    # 删除 Pod。
    result = delete_pods(list2)

    return result


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)



import os
import yaml
import kubernetes
import base64

with open(os.environ["KUBECONFIG"], "r"):
    kubeconfig = yaml.load(fd.read(), yaml.FullLoader)

cluster = kubeconfig["clusters"][0]
ca_b64 = cluster["cluster"]["certificate-authority-data"]
host = cluster["cluster"]["server"]
user = kubeconfig["users"][0]
token = user["user"]["token"]
ca = base64.b64decode(ca_b64.encode("utf-8").decode("utf-8")
with open("ca.crt", "w") as fd:
  fd.write(ca) 
configuration = kubernetes.client.Configuration()
configuration.host = host
configuration.api_key = {"authorization": token}
configuration.api_key_prefix = {"authorization": "bearer"}
configuration.ssl_ca_crt = "ca.crt"
# The above does not work so I still needed to disable SSL
configuration.verify_ssl = False
client = kubernetes.client.ApiClient(configuration)
v1 = kubernetes.client.CoreV1Api(client)
v1.list_node()


###
import random
import time
import threading
import numpy as np
from milvus import Milvus, IndexType
from milvus.client.types import MetricType

SERVER_ADDR = "127.0.0.1"
SERVER_PORT = '19531'
COLLECTION_NAME = "TEST"
COLLECTION_DIMENSION = 450
COLLECTION_PARAM = {'collection_name': "",
                    'dimension': COLLECTION_DIMENSION,
                    'index_file_size': 1024,
                    'metric_type':MetricType.L2}
INDEX_TYPE = IndexType.IVF_SQ8
INDEX_PARAM = {'nlist': 512}

MILVUS = Milvus(host=SERVER_ADDR, port=SERVER_PORT)

def gen_vec_list(nb, seed=np.random.RandomState(1234)):
    xb = seed.rand(nb, COLLECTION_DIMENSION).astype("float32")
    vec_list = xb.tolist()
    return vec_list

def create_collection(name = COLLECTION_NAME):
    param = COLLECTION_PARAM
    param['collection_name'] = name
    status, has = MILVUS.has_collection(collection_name=name)
    if not has:
        status = MILVUS.create_collection(param)
        MILVUS.create_index(collection_name=name, index_type=INDEX_TYPE, params=INDEX_PARAM)
        print("create", name)

def drop_collection(name = COLLECTION_NAME):
    status = MILVUS.drop_collection(collection_name=name)
    print("drop", name)

def get_collection_stats(name = COLLECTION_NAME):
    status, stat = MILVUS.get_collection_stats(collection_name=name)
    print(stat)

def insert(name = COLLECTION_NAME):
    batch = 100
    for i in range(100000):
        vectors = gen_vec_list(batch)
        status, ids = MILVUS.insert(collection_name=name, records=vectors)
        print("insert", i, len(ids))
        time.sleep(0.2)

if __name__ == "__main__":
    create_collection()
    insert()

###
import random
import time
import threading
import numpy as np
from milvus import Milvus, IndexType
from milvus.client.types import MetricType

SERVER_ADDR = "127.0.0.1"
SERVER_PORT = '19531'
COLLECTION_NAME = "TEST"

NQ = 1
SEARCH_PARAM = {'nprobe': 4}

MILVUS = Milvus(host=SERVER_ADDR, port=SERVER_PORT)

def search():
    for i in range(1000000):
        q_records = [[random.random() for _ in range(COLLECTION_DIMENSION)] for _ in range(1)]
        topk = random.randint(20,1200)
        status, results = MILVUS.search(collection_name=COLLECTION_NAME, query_records=q_records, top_k=topk,
                                        params=SEARCH_PARAM)
        print("search", i, 'tok=', topk)
        if not status.OK():
            break

if __name__ == "__main__":
    threads = []
    for k in range(10):
        x = threading.Thread(target=search, args=())
        threads.append(x)
        x.start()

    for th in threads:
        th.join()

