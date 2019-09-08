# AnEngine

&#8195;&#8195;AnEngine是一个使用DirectX 12开发的尚未完成的游戏引擎。之前的作用是完成图形学作业，现在没什么作用。
目前已经支持C++ 17标准。

## 特征
* 使用了 ECS + Behaviour 的设计，由 Scene 统一管理，Engine 调度。
* 游戏中的一个对象是一个 GameObject，保存在 Scene 中，Component 放在连续的内存中，可以通过下标索引，通过下标保证和 GameObject 的对应关系；Behaviour 针对少量、复用很少的组件，挂在 GameObject 上。

## 正在开发的功能
* 丧心病狂的开始尝试DXR（受到 Windows 10 1809 更新推迟影响，暂时停止开发）
* ECS-Behaviour 模式
* 重写可编程渲染管线

### 已完成功能
* Sample Particles Renderer（CS计算位置，GS和PS控制形状）
* Sample Mesh Renderer（由于管线重写，该项和上一项废弃）
* MSAA（DX API实现）
* State Machine
* D2D

#### ECS-Behaviour
目前只实现了 Transform 系列，如坐标变换功能。System 有两种，一种是可并行的，继承自 IParaller，另一种是不可并行的。
* IParaller
``` c++
class IParaller
{
public:
	bool Check() = 0;
	void Execute(size_t index) = 0;
	size_t Length;
}
```
Check中判断当前 index 的 GameObject 是否可被该 System 执行，每次执行都检查会降低效率，但目前没有找到更好的办法。

内存分布（以 Transform 为例）：

|index|0|1|....|
|-----------|-------|-------|----|
|GameObject*|objPtr0|objPtr1|....|
|Position|pos0|pos1|....|
|Rotation|rot0|rot1|....|
|........|||
如果某个 GameObject 没有 Component，则对应的 Component 应令其 active = false，且在 System 的 Check() 中进行检查。
如果销毁某个 GameObject，则将对应的 GameObject 标记为以销毁，并在 Check() 中执行检查。如果新建一个 GameObject，则优先使用已销毁位置的 Component 和 GameObject。
* ISerial

对于不可并行却要用 ECS System 的，继承自该类。但对于串行执行的，一般都有彼此间的依赖，如果数量不多建议使用 Behaviour。
* ObjectBehaviour

对于 ECS 模式，数据和逻辑是分离的，但对于 Behaviour，数据和逻辑是放在一起的。其中 BeforeUpdate() 在 System 调度前调用，Update()和 LateUpdate() 在 System 调度后调用。

#### 状态机
状态机(State Machine)在命名空间AnEngine::Game中，继承自 ObjectBehaviour 类。一个状态机包括参数、状态和转移条件。在引入 ECS 后，状态机暂时只针对 Behaviour 起作用。
* 参数
参数是可以从外部设置的状态机内部变量，由 int、float、bool和Trigger 四种类型。其中Trigger是特殊的bool，只能从外部设置为true，使用后自动变为false。
``` cpp
void SetInt(std::wstring&& name, int value);
void SetBool(std::wstring&& name, bool value);
void SetFloat(std::wstring&& name, float value);
void SetTrigger(std::wstring&& name);
```
* 状态
状态是StateMachine的内部类，其属性为private，既不能从外部实例化，只能通过StateMachine中的成员函数CreateNewState实例化，该函数返回新建状态在该状态机的index。状态实际上包装了一个std::function<void()>对象，考虑到状态机实际运行和编码难度，没有实现可变返回类型和参数类型。
``` cpp
int CreateNewState(std::wstring&& name, const std::function<void()>& func);
int CreateNewState(std::wstring&& name, std::function<void()>&& func);
```
* 转移条件
从一个状态转移到另一个状态需要满足两个状态之间的所有转移条件。转移条件可以通过StateMachine中的CreateStateTransCondition函数来创建，通过指定转移方向、条件参数和条件，其中条件是StateMachine的内部枚举Condition，包括Equal、NotEqual、Less、LessOrEqual、Greater、GreaterOrEqual几种。
``` cpp
void CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, uint32_t newValue, Condition cond);
void CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, float newValue, Condition cond);
void CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, bool newValue, Condition cond);
void CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& tiggerName);
```

## 系统要求
* Windows 10 17134或更新
* Visual Studio 2017，需要 C++ 游戏开发组件、Windows 10 SDK 17134、VC++ ATL
* [Fallback Layer for DXR](https://github.com/Microsoft/DirectX-Graphics-Samples)

## 开始
在Window项目中新建头文件和源文件（Test.h，Test.cpp），头文件中引用 Driver.h。
* Test.h
``` cpp
#pragma once
#include "Driver.h"

void LoadScene();

class TestCamera : public AnEngine::Game::ObjectBehaviour
{
protected:
	virtual void Update() override;
public:
	TestCamera();
};

```
&#8195;&#8195;为了保证最基本的图形渲染，我们需要一个场景，并在场景中添加照相机。因此在Test.h中声明了一个函数LoadScene()和一个类TestCamera。TestCamera继承自ObjectBehaviour，表示这是场景中一个组件。这里测试状态机类StateMachine
* Test.cpp
``` cpp
#include "Test.h"
using namespace AnEngine;
using namespace AnEngine::Game;
using namespace AnEngine::RenderCore;

Scene* testScene;
Camera* testCamera;

void LoadScene()
{
	testScene = new Scene(L"Test Scene");
	GameObject* testCameraObject = new GameObject(L"Test Object");

	testCamera = new Camera();
	testCamera->ClearFlag(Camera::ClearFlags::SolidColor);
	testCamera->ClearColor(Color::Black);
	testCameraObject->AddComponent(testCamera);

	TestCamera* cameraScript = new TestCamera();
	testCameraObject->AddComponent(cameraScript);

	StateMachine* fsm = new StateMachine();
	int s1 = fsm->CreateNewState(L"State1", [] {});
	int s2 = fsm->CreateNewState(L"State2", [] {});
	fsm->AddFloatParam(L"Sin", 0f);
	fsm->AddTrigerParam(L"Trigger");
	fsm->CreateStateTransCondition(s1, s2, L"Sin", 0f, StateMachine::Condition::LessOrEqual);
	fsm->CreateStateTransCondition(s2, s1, L"Sin", 0f, StateMachine::Condition::Greater);
	testCameraObject->AddComponent(fsm);

	// 使用D2D显示文字
	GameObject* canvas = new GameObject(L"Canvas");
	canvas->AddComponent<Canvas>();
	GameObject* debugText = new GameObject(L"Text");
	debugText->AddComponent<UIText>();
	debugText->GetComponent<UIText>()->Text(L"Test");
	canvas->AddChildObject(debugText);

	testScene->AddObject(testCameraObject);
	Driver::GetInstance()->StartScene(testScene);
}

void TestCamera::Update()
{	
	var stateMachine = gameObject->GetComponent<StateMachine>();
	stateMachine->SetFloat(L"Sin", sin((float)Timer::GetTotalTicks() / 240000));
	GameObject::Find(L"Text")->GetComponent<UIText>()->Text(stateMachine->GetCurrectStateName());
}

TestCamera::TestCamera() : ObjectBehaviour() { }
```
&#8195;&#8195;在CPP文件中对TestCamera的构造函数进行定义，这个对象并没有什么用，所以不需要在构造函数中做任何事情，只需要调用基类构造函数即可。<br/>
&#8195;&#8195;在LoadScene()中，我们创建了一个空场景testScene，一个照相机testCamera，将清空标志设置为纯色，可以通过Update()实时改变填充颜色。然后实例化一个TestCamera对象cameraScript，将脚本作为该对象的组件，即调用testCameraObject的AddComponent方法将testCamera和testCameraScript挂上去。然后创建一个状态机，设置两个状态State1和State2，当Sin值大于0的时候是State1，否则是State2。最后，将testCameraObject对象加入到场景中，使用Driver的BeginBehaviour()方法加载场景。

* WinMain.cpp：
```cpp
......
ShowWindow(window, nCmdShow);

LoadScene();

MSG msg = {};
while (msg.message != WM_QUIT)
{
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
......
```

### 第三方引用说明
* [OpenFBX](https://github.com/nem0/OpenFBX)
* [D3D12Raytracing](https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12Raytracing/readme.md)

### 由于某些原因，暂停开发，明年再说