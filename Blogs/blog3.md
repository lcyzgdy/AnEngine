# 一个引擎
上一篇新建了一个D3D程序的基本架构D3D12AppBase，那么如果想让程序运行，只需要用一个自定义类继承D3D12AppBase，从中添加逻辑即可。
### 渲染管线
对于DX来说，如果想要渲染一个三角形，其在显卡上要通过“渲染管线”来进行渲染。渲染管线就像生产流水线一样，将渲染分阶段进行，不同阶段可以并行进行，大大提高了硬件利用率和程序效率。

D3D的渲染管线：局部坐标变换 -> 世界坐标变换 -> 观察坐标变换 -> 背面消除 -> 光照 -> 裁剪 -> 投影 -> 视口计算 -> 光栅化

对于现代显卡来说，渲染管线大部分工作可以通过硬件完成。抛弃坐标变换、背面剔除等工作，将一个三角形渲染到屏幕上，令其坐标都在屏幕中，那么显卡的输入是三角形顶点的坐标和颜色，输出为光栅化的像素。所以在渲染管线中，GPU的处理流程是可以通过Shader控制的。

* Vertex Shader：顶点着色器，在坐标变换和光照计算时工作。输入一般为顶点坐标和颜色。
* Pixel Shader：像素着色器，有时也成为Fragment Shader（片元着色器），在光栅化时工作，输入是顶点着色器的输出，输出一定格式的颜色。
* Geometry Shader：几何着色器，位于VS和PS之间，可以利用GPU进行几何变换来补充3D对象的几何细节。比如对于某些粒子，可以用一个顶点来表示其属性，但一个顶点渲染出来是看不到的，所以可以用几何着色器将其变为4个顶点，既一个矩形，然后再到PS中继续处理。
* Hull Shader：外壳着色器（？？？），和曲面细分有关。主要负责定义细分等级（LOD）和相关控制点在细分中的“形变”趋势，需要说明的是这种形变仅仅是类似于曲率改变等小幅度的变化，而非大幅度的多边形位移。
* Domain Shader：域着色器，也和曲面细分有关。通过贴图控制的方式，实现模型的形变。这两个着色器在VS和PS之间，GS之前。
* Compute Shader：计算着色器，不属于图形流水线，但可以在图形流水线之前使用CS处理数据。其作用在于GPU并行计算。

以上着色器都是针对GPU开发，所以是并行程序。在编写的时候，针对的是单个数据。例如在渲染三角形时，VS的输入是三角形的三个顶点坐标和颜色，那么在VS编写时写的是处理一个顶点的代码，GPU会对所有输入的代码运行该Shader。

因此，渲染管线可以用另一种描述：VS -> HS -> DS -> GS -> PS。

* 固定渲染管线：渲染管线的各个阶段是固定的，最多可以编写不同阶段的Shader。
* 可编程渲染管线：渲染管线的各个阶段是可以控制的。在接下来的代码中，只用到了VS和PS，没有中间几个阶段，简化了流水线，程序效率更高。

### 同步
DirectX 12中，使用Commmand List记录渲染命令，向Command Queue提交渲染命令，Command Queue向显卡提交渲染命令。在CPU端，Command Queue提交命令后会立刻返回，GPU按Command Queue中提交的命令执行。其并行执行机制后文再说明。所以Command Queue提交命令后，CPU并不知道GPU执行这些命令需要多久，而为了安全，GPU和CPU不能同时写某个内存地址对应的数据，因此需要在GPU和CPU间做同步，以让一方等待另一方的工作状态。

### 另一个辅助类
显然在开始渲染之前要先定义渲染管线和载入资源，并在渲染过程中进行同步。因此定义了另一个辅助类D3D12Base
``` cpp
class D3D12Base
{
protected:
	virtual void InitializePipeline() = 0;
	virtual void InitializeAssets() = 0;
	virtual void WaitForGpu() = 0;
	virtual void PopulateCommandList() = 0;
	virtual void MoveToNextFrame() = 0;
	virtual void WaitForRenderContext() = 0;
public:
	D3D12Base() = default;
	~D3D12Base() = default;
}
```
其中包括初始化管线、载入资源、GPU与CPU间同步、CPU向GPU提交渲染命令的接口。

### 清空渲染区域
假设渲染区域是一块幕布，那么在绘制某一帧的时候，要先把上一帧在上面绘制的图像清空。

HelloWorld.h
``` cpp
class HelloWorld : public D3D12AppBase, public D3D12Base
{
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> renderTarget;
	UINT rtvDescriptorSize;

public:
	HelloWorld(const HWND _hwnd, const UINT _width, const UINT _height);
	~HelloWorld() = default;

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

	virtual void OnKeyUp(UINT8 _key);
	virtual void OnKeyDown(UINT8 _key);

	virtual void InitializePipeline();
	virtual void InitializeAssets();
	virtual void PopulateCommandList();
	virtual void WaitForGpu();
	virtual void MoveToNextFrame();
	virtual void WaitForRenderContext();
}
```
HelloWorld.cpp
``` cpp
HelloWorld::HelloWorld(const HWND _hwnd, const UINT _width, const UINT _height) :
	D3D12AppBase(_hwnd, _width, _height), rtvDescriptorSize(0)
{
}

void HelloWorld::OnInit()
{
	rtvDescriptorSize = 0;

	InitializePipeline();
	InitializeAssets();
}

void HelloWorld::OnUpdate()
{
}

void HelloWorld::OnRender()
{
	PopulateCommandList();	//填入命令
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);	//提交命令
	swapChain->Present(1, 0);	//将渲染区域由内存或显存呈现到显示器上
}

void HelloWorld::OnRelease()
{
}

void HelloWorld::OnKeyUp(UINT8 _key)
{
}

void HelloWorld::OnKeyDown(UINT8 _key)
{
}

void HelloWorld::InitializePipeline()
{
	UINT dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> d3dDebugController;
	if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
	{
		d3dDebugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	// 开启Debug模式

	ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	if (isUseWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
		D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}
	// 创建设备

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	// 描述并创建命令队列

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = 1;	// 交换链缓冲区，此时没有使用双缓冲，所以是1
	swapChainDesc.Width = this->width;
	swapChainDesc.Height = this->height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(
		commandQueue.Get(),		// 交换链需要队列，以便可以强制刷新它
		hwnd, &swapChainDesc, nullptr, nullptr,	&swapChain1));
	// 描述并创建交换链

	factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
	swapChain1.As(&swapChain);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = r_DefaultFrameCount_const;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	// 创建渲染目标视图描述符堆

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// 创建描述符堆

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	swapChain->GetBuffer(0, IID_PPV_ARGS(&renderTarget));
	device->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);
	rtvHandle.Offset(1, rtvDescriptorSize);

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	// 创建命令分配器和创建帧资源
}

void HelloWorld::InitializeAssets()
{
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	// 创建命令列表

	commandList->Close();
	// 只有命令列表处于关闭状态才能向命令队列
}

void HelloWorld::PopulateCommandList()
{
	commandAllocator->Reset();
	// 命令分配器只能在关联的命令列表已在GPU上完成执行时重置，应用程序应该使用栅栏来确定GPU执行进度。

	commandList->Reset(commandAllocator.Get(), nullptr);
	// 当在特定命令列表上调用ExecuteCommandList()时，该命令列表随后可以在重新记录之前。任何时间重置，

	// 设置必要的状态。
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.2f, 0.2f, 0.6f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}
void HelloWorld::WaitForGpu()
{
}
void HelloWorld::MoveToNextFrame()
{
}
void HelloWorld::WaitForRenderContext()
{
}
```
以上代码的作用就是将屏幕清空为（0.2, 0.2, 0.6）所表示的颜色。

在InitializePipeline()中，最主要的工作不是创建管线，而是初始化硬件。
首先需要IDXGIFactory4接口，利用CreateDXGIFactory2()初始化这个接口。接下来枚举硬件，然后调用factory的CreateCommandQueue接口创建一个ID3D12Device和一个交换链，CreateSwapChainForHwnd()创建的是针对Win32窗口的交换链，这点前文已经提过。有了设备和交换链后，factory的任务就完成了，这时将其释放即可。

然后通过填充struct D3D12_COMMAND_QUEUE_DESC，描述一个命令队列，然后调用设备的CreateCommandQueue()接口创建命令队列。对于一个设备，可以抽象出图形、计算、拷贝、视频编码、视频解码等多个引擎（现代显卡的多引擎架构），每个引擎只能有一个命令队列，这里用 D3D12_COMMAND_LIST_TYPE_DIRECT 表示这是图形引擎。

接下来通过填充struct D3D12_DESCRIPTOR_HEAP_DESC，描述一个RTV的资源描述符堆。RTV（Render Target View）表示当前资源是渲染目标视图。渲染区域、纹理、缓冲都可以是RTV，RTV的Handle放在Descriptor Heap中统一管理。类似的还有SRV（Shader Resource View，着色器资源视图）、UAV（Unordered Access View，无序访问视图）、DSV（Depth Stencil View，深度模板视图）等。这里先用到一个RTV。

然后从交换链上获取缓冲，这里没有用到双缓冲，所以只获取到一个。所有资源使用ID3D12Resource，然后为该Resource创建RTV。最后创建一个命令分配器。

在LoadAsset()中只创建一个命令列表。命令列表的作用是记录向GPU提交的命令，因此可以创建多个，而且可以对应不同的引擎。

PopulateCommandList()用来向命令列表记录命令，首先需要将命令分配器和命令列表重置，ResourceBarrier()的作用是转换资源屏障，既一个资源有多种状态，想要向上面渲染其状态应为Render Target，而想要让交换链呈现到屏幕上其状态应为Present，也起到了资源同步的作用。