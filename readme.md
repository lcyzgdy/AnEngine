# AEngine

&#8195;&#8195;AEngine是使用DirectX 12开发的游戏引擎。之前的作用是完成图形学作业。

## Namespace
### RenderCore
#### GraphicCard
与计算机上的显卡进行交互的类。若使用Linked Gpus或者Multi Adapter特性，则每个显卡分别对应一个GraphicCard类。目前只考虑一个显卡的情况。
* ComPtr< ID3D12Device1 > m_device
* CommandQueue


### 已完成Demo
* 绘制三角形
* 绘制多边形
* 绘制任意斜率直线
* 使用Wu-反走样算法绘制任意斜率直线
* NBody Demo
* 绘制顶点为随机颜色的立方体