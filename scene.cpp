#include "scene.h"
#include "BVulkan.h"
#include "XVulkan.h"

AVulkanHandle program;
XBufferObject* vbo;
void Init()
{
	// 目前使用的都是NDC空间坐标, 逆时针绘制
	Vertex vertexes[3];
	vertexes[0].SetPosition(-0.5f, 0.5f, 0.0f);
	vertexes[0].SetTexcoord(1.0f, 0.0f, 1.0f, 1.0f);
	vertexes[1].SetPosition(0.5f, 0.5f, 0.0f);
	vertexes[1].SetTexcoord(1.0f, 1.0f, 0.0f, 1.0f);
	vertexes[2].SetPosition(0.0f, -0.5f, 0.0f);
	vertexes[2].SetTexcoord(0.0f, 1.0f, 1.0f, 1.0f);
	vbo = new XBufferObject;
	xglBufferData(vbo, sizeof(Vertex) * 3, vertexes);// 构建点集数据
	program = aCreateProgram();// 创建应用

	GLuint vs, fs;
	int file_len = 0;
	unsigned char* file_content = LoadFileContent("Res/test.vsb", file_len);// 先加载这个二进制顶点shader
	aCreateShader(vs, file_content, file_len);// 创建出顶点shader
	delete[]file_content;// 用完这个VSshader就可以把文件删了
	file_content = LoadFileContent("Res/test.fsb", file_len);// 重用资源, 再加载这个二进制像素shader
	aCreateShader(fs, file_content, file_len);// 创建出像素shader
	delete[]file_content;// 用完这个FSshader就可以把文件删了

	// 各自绑到管线上 并最终链接程序
	aAttachVertexShader(program, vs);
	aAttachFragmentShader(program, fs);
	aLinkProgram(program);
}

void Draw(float deltaTime)
{
	aClearColor(0.1f, 0.4f, 0.6f, 1.0f);

	VkCommandBuffer commandbuffer = aBeginRendering(); /*******开始绘制标志******/
	glUseProgram(program);
	glBindVertexBuffer(vbo);
	glDrawArrays(A_TRIANGLES/*图元类型*/, 0/*数据偏移*/, 3/*欲绘制的点个数*/);
	aEndRenderingCommand();/*******结束绘制标志******/
	aSwapBuffers();// 绘制完就交换前后缓冲
}

void OnViewportChanged(int width, int height)
{
	aViewport(width, height);// 当拖动窗口或者其他改变尺寸操作时, 就需要重适配视口
}

void OnQuit()
{
	if (program != nullptr) {
		aDeleteProgram(program);
	}
	if (vbo != nullptr) {
		glDeleteBufferObject(vbo);
	}
}
