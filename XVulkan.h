#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
typedef void* XVulkanHandle;// 定义1个空指针对象, 允许指向任意对象

struct XBufferObject
{
	VkBuffer mBuffer;// 缓冲区,表示逻辑缓存对象
	VkDeviceMemory mMemory;//  真实的物理空间
	XBufferObject();
	~XBufferObject();
};

/* 把数据从CPU传入GPU,构建某型buffer的数据
 * (一般用于构建顶点点集)
 */ 
void xglBufferData(XVulkanHandle vbo, int size, void* data);

/* 申请各类型buffer的显存区域*/
VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,/*欲申请显存的尺寸*/ 
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties/*用以判断更靠近CPU还是GPU*/);

/* 依赖上一行定义某类型buffer的申请显存宏 
 * 此宏是xGenBuffer函数的变种
 * 定义VBO型buffer的申请显存
 */
#define xGenVertexBuffer(size, buffer, buffermemory) \
		xGenBuffer(buffer, buffermemory, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT/*单次数据传输的终点*/, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT/*仅供显卡访问,CPU禁止访问*/ )
