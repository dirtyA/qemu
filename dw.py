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