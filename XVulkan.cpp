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
	xBufferSubVertexData(vbo->mBuffer, data, size);// 传顶点数据从CPU端到GPU端
}

void xBufferSubData(VkBuffer buffer, VkBufferUsageFlags usage/*不知道用途,可能是索引或者顶点*/, const void* data, VkDeviceSize size)
{
	VkBuffer tempbuffer;
	VkDeviceMemory tempmemory;
	/* 为临时内存申请显存, 和之前不让CPU看到的VBO有所区别的是,此处这块区域是允许CPU可见可写的*/
	xGenBuffer(tempbuffer, tempmemory, size, usage,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	/* 映射和解映射出一块地址用于承接数据源*/
	void* host_memory;
	vkMapMemory(GetVulkanDevice(), tempmemory, 0, size/*要写多大数据*/, 0, &host_memory/*要写入的地址*/);
	memcpy(host_memory, data, (size_t)size);
	vkUnmapMemory(GetVulkanDevice(), tempmemory);
	//==============截止到这里,就把数据源从CPU拷贝到了一个临时空间里了============

	//====CommandBuffer是沟通CPU和GPU之间的桥梁=========
	//====CommandBuffer此处负责把数据源从临时memory拷贝到VBO里面去
	VkCommandBuffer commandbuffer;
	aBeginOneTimeCommandBuffer(&commandbuffer);// 注明只使用一次commandbuffer
	VkBufferCopy copy = { 0,0,size };
	/* 数据从临时区域拷贝到VBO*/
	vkCmdCopyBuffer(commandbuffer, tempbuffer/*来源buffer*/, buffer/*目的buffer,也就是VBO*/, 1, &copy);
	aEndOneTimeCommandBuffer(commandbuffer);

	vkDestroyBuffer(GetVulkanDevice(), tempbuffer, nullptr);//清除资源tempbuffer
	vkFreeMemory(GetVulkanDevice(), tempmemory, nullptr);//释放内存

	//+++以上执行细节是先把数据从CPU端拷贝到某个临时区域,再把临时区域借助某些系统总线拷贝至GPU端+++
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
	// 至于vulkan数据的结构设计有关,与其他原理没有关系
	memoryallocinfo.memoryTypeIndex = xGetMemoryType(requirements.memoryTypeBits, properties);// 获取并筛选一下申请显存的类型索引,即显存用途,和入参亦有关联
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

uint32_t xGetMemoryType(uint32_t type_filters, VkMemoryPropertyFlags properties)
{
	/* 首先拿到物理设备内存的一些属性, 而非逻辑设备的!!!!!*/
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(GetVulkanPhysicalDevice(), &memory_properties);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
		uint32_t flag = 1 << i;
		if ((flag & type_filters) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;//满足两组与关系的就是符合需求的,提取出这个索引
		}
	}
	return 0;// 没找到合适的索引否则就正常退出
}
