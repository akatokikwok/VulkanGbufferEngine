#include "XVulkan.h"
#include "BVulkan.h"
XBufferObject::XBufferObject()
{
	mBuffer = 0;
	mMemory = 0;
}

XBufferObject::~XBufferObject()
{
	if (mBuffer != 0) {
		vkDestroyBuffer(GetVulkanDevice(),/*显卡的逻辑设备*/ mBuffer, nullptr/*管理资源的回调函数*/);
	}
	if (mMemory != 0) {
		vkFreeMemory(GetVulkanDevice(), mMemory, nullptr);
	}
}

void xglBufferData(XVulkanHandle buffer, int size, void* data)
{
	XBufferObject* vbo = (XBufferObject*)buffer;
	xGenVertexBuffer(size, vbo->mBuffer, vbo->mMemory);// 给字段mBuffer开辟空间大小, 用以申请显存
	aBufferSubVertexData(vbo->mBuffer, data, size);// 把数据传上去顶点数据
}

VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,/*欲申请显存的尺寸*/
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties/*用以判断更靠近CPU还是GPU*/)
{
	VkBufferCreateInfo bufferinfo = {};// 创建某类型的createInfo
	bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferinfo.size = size;
	bufferinfo.usage = usage;
	/* 创建出指定的buffer*/ 
	VkResult ret = vkCreateBuffer(GetVulkanDevice(), &bufferinfo, nullptr, &buffer);
	if (ret != VK_SUCCESS) {
		printf("未能成功创建buffer\n");
		return ret;
	}

	VkMemoryRequirements requirements;
	/* 依赖创造出的buffer给requirements结构体里写值,即获取到buffer对内存的一些需求*/
	vkGetBufferMemoryRequirements(GetVulkanDevice(), buffer, &requirements);
	VkMemoryAllocateInfo memoryallocinfo = {};
	memoryallocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryallocinfo.allocationSize = requirements.size;// 分配的显存有个特性,是按照256字节对齐的,所以这里要特殊处理
	memoryallocinfo.memoryTypeIndex = FindMemoryType(requirements.memoryTypeBits, properties);// 设置一下申请显存的类型索引,即显存用途,和入参亦有关联
	/* 利用AllocateInfo结构体和入参, 真正分配出显存*/
	ret = vkAllocateMemory(GetVulkanDevice(), &memoryallocinfo, nullptr, &buffermemory);
	if (ret != VK_SUCCESS) {
		printf("分配显存失败\n");
		return ret;
	}
	/* 把分配出的内存和所需buffer关联到一起*/
	vkBindBufferMemory(GetVulkanDevice(), buffer, buffermemory, 0);

	return VK_SUCCESS;
}
