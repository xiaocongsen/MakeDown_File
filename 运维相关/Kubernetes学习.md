# Kubernetes 组建
![图片](http://dockone.io/uploads/article/20151230/d56441427680948fb56a00af57bda690.png)
- Pod

    包含一组容器和卷。同一个Pod里的容器共享同一个网络命名空间，可以使用localhost互相通信。Pod是短暂的，不是持续性实体。

- Label(label)（标签）

    一个Label是attach到Pod的一对键/值对，用来传递用户定义的属性。
    比如，你可能创建了一个"tier"和“app”标签，通过Label（tier=frontend, app=myapp）来标记前端Pod容器，使用Label（tier=backend, app=myapp）标记后台Pod。然后可以使用Selectors选择带有特定Label的Pod，并且将Service或者Replication Controller应用到上面。

- Replication Controller（复制控制器）

    Replication Controller确保任意时间都有指定数量的Pod“副本”在运行。如果为某个Pod创建了Replication Controller并且指定3个副本，它会创建3个Pod，并且持续监控它们。如果某个Pod不响应，那么Replication Controller会替换它，保持总数为3。
    当创建Replication Controller时，需要指定两个东西：
    1. Pod模板：用来创建Pod副本的模板
    2. Label：Replication Controller需要监控的Pod的标签。

- Service（enter image description here）（服务）

    Service是定义一系列Pod以及访问这些Pod的策略的一层抽象。Service通过Label找到Pod组。因为Service是抽象的，所以在图表里通常看不到它们的存在，这也就让这一概念更难以理解。

    假定有2个后台Pod，并且定义后台Service的名称为‘backend-service’，lable选择器为（tier=backend, app=myapp）。backend-service 的Service会完成如下两件重要的事情：
    1. 会为Service创建一个本地集群的DNS入口，因此前端Pod只需要DNS查找主机名为 ‘backend-service’，就能够解析出前端应用程序可用的IP地址。
    2. 现在前端已经得到了后台服务的IP地址，但是它应该访问2个后台Pod的哪一个呢？Service在这2个后台Pod之间提供透明的负载均衡，会将请求分发给其中的任意一个（如下面的动画所示）。通过每个Node上运行的代理（kube-proxy）完成

    有一个特别类型的Kubernetes Service，称为'LoadBalancer'，作为外部负载均衡器使用，在一定数量的Pod之间均衡流量。比如，对于负载均衡Web流量很有用。

- Node（节点）
    节点（上图橘色方框）是物理或者虚拟机器，作为Kubernetes worker，通常称为Minion。每个节点都运行如下Kubernetes关键组件：
    1. Kubelet：是主节点代理。
    2. Kube-proxy：Service使用其将链接路由到Pod，如上文所述。
    3. Docker或Rocket：Kubernetes使用的容器技术来创建容器。

- Kubernetes Master（Kubernetes主节点）   

    集群拥有一个Kubernetes Master（紫色方框）。Kubernetes Master提供集群的独特视角，并且拥有一系列组件，比如Kubernetes API Server。API Server提供可以用来和集群交互的REST端点。master节点包括用来创建和复制Pod的Replication Controller。



# 安装
安装minikube
curl -Lo minikube https://storage.googleapis.com/minikube/releases/v0.15.0/minikube-darwin-amd64 && chmod +x minikube && sudo mv minikube /usr/local/bin/
安装kubectl
curl -Lo kubectl http://storage.googleapis.com/kubernetes-release/release/v1.5.1/bin/darwin/amd64/kubectl && chmod +x kubectl && sudo mv kubectl /usr/local/bin/

1. 创建并启动 minikube 虚拟机

        $ minikube start
        Starting local Kubernetes cluster...
        Running pre-create checks...
        Creating machine...
        Starting local Kubernetes cluster...

2. 创建 hello-minikube 部署

        $ kubectl run hello-minikube --image=tomcat:8.0 --port=8080
        deployment "hello-minikube" created

3. 发布服务 hello-minikube

        $ kubectl expose deployment hello-minikube --type=NodePort
        service "hello-minikube" exposed

4. 查看 pods

        $ kubectl get pods
        NAME                             READY     STATUS              RESTARTS   AGE
        hello-minikube-598805112-3bzmf   1/1       ContainerCreating   0          5s
        注意：刚开始时，pod 没有完全创建好的时候，状态是 ContainerCreating，当部署完成后，状态就变成 Running。

        $ kubectl get pods
        NAME                             READY     STATUS    RESTARTS   AGE
        hello-minikube-598805112-3bzmf   1/1       Running   0          25s

5. 获取服务地址

        $ minikube service hello-minikube --url
        http://192.168.99.102:30724
        $ minikube service hello-minikube 将直接打开地址到默认浏览器上。

6. 停止 minikube 虚拟机

        $ minikube stop
        Stopping local Kubernetes cluster...
        Stopping "minikube"...


#实践v

## 创建Minikube集群
1. 准备工作
- 安装xhyve驱动

        brew install docker-machine-driver-xhyve
        sudo chown root:wheel $(brew --prefix)/opt/docker-machine-driver-xhyve/bin/docker-machine-driver-xhyve
        sudo chmod u+s $(brew --prefix)/opt/docker-machine-driver-xhyve/bin/docker-machine-driver-xhyve

- NodeJS
- 在OS X上安装Docker，推荐 Docker for Mac。

2. 使用
- 使用curl下载并安装最新版本Minikube：
curl -Lo minikube https://storage.googleapis.com/minikube/releases/latest/minikube-darwin-amd64 && chmod +x minikube && sudo mv minikube /usr/local/bin/

- 使用Homebrew安装xhyve驱动程序并设置其权限