from fastapi import FastAPI, Body
import kubernetes.client

app = FastAPI()

@app.get("/pods")
async def list_pods():
    """
    获取命令空间为www下所有的pod名称。

    :return: pod 名称列表。
    """

    configuration = kubernetes.client.Configuration()
    # Configure API key authorization: BearerToken
    configuration.api_key['authorization'] = 'YOUR_API_KEY'

    with kubernetes.client.ApiClient(configuration) as api_client:
        api_instance = kubernetes.client.CoreV1Api(api_client)

        # 获取 pod 列表。
        pods = api_instance.list_namespaced_pod("www")

        # 返回 pod 名称列表。
        return pods.items


def delete_pods(pod_names: List[str]):
    """
    删除 Pod。

    :param pod_names: Pod 的名称列表。
    :return: 删除 Pod 的结果。
    """

    configuration = kubernetes.client.Configuration()
    # Configure API key authorization: BearerToken
    configuration.api_key['authorization'] = 'YOUR_API_KEY'

    with kubernetes.client.ApiClient(configuration) as api_client:
        api_instance = kubernetes.client.CoreV1Api(api_client)

        # 删除 Pod。
        try:
            for pod_name in pod_names:
                api_response = api_instance.delete_namespaced_pod(
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
