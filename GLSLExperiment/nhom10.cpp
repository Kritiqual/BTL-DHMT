#include "Angel.h"

void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

const int NumPoints = 36;

point4 points[NumPoints];
color4 colors[NumPoints];
vec3 normals[NumPoints];


point4 vertices[8];
color4 vertex_colors[8];

GLuint program;

mat4 instance;
mat4 model, view, projection;
GLuint model_loc, view_loc, projection_loc;
GLfloat mauAnhSang = 1.0;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(0.0, 1.0, 1.0, 1.0); // cyan
	vertex_colors[7] = color4(1.0, 1.0, 1.0, 1.0); // white
}
int Index = 0;
void quad(int a, int b, int c, int d)
/*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;

}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(5, 4, 0, 1);
	quad(1, 0, 3, 2);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(3, 0, 4, 7);
	quad(2, 3, 7, 6);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}

void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

}
void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/

}

// Các hàm được sử dụng nhiều
void Paint(GLfloat a, GLfloat b, GLfloat c) {
	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(10.0, 10.0, 10.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(mauAnhSang, mauAnhSang, mauAnhSang, 1.0);
	color4 light_specular(0.0, 0.0, 0.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(a / 255.0, b / 255.0, c / 255.0, 1.0);
	color4 material_specular(1.0, 1.0, 1.0, 1.0);
	float material_shininess = 50.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}
void block(GLfloat x, GLfloat y, GLfloat z, GLfloat sx, GLfloat sy, GLfloat sz) {
	instance = Translate(x, y, z) * Scale(sx, sy, sz);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void wheel(GLfloat x, GLfloat y, GLfloat z, GLfloat thick, GLfloat size) {
	Paint(0, 0, 0);
	mat4 instance;
	for (int i = 0; i < 360; i++) {
		instance = Translate(x, y, z) * RotateX(i) * Scale(thick, size, size);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

// Tuong rao va San cong truong
GLfloat rtCTX = 0, rtCTY = 0, rtCTZ = 0;
void veAllTuong() {
	// Tuong sau
	block(0, 0.25, -4.99, 10, 0.5, 0.02);
	// Tuong truoc
	block(0, 0.25, 4.99, 10, 0.5, 0.02);
	// Tuong phai
	block(4.99, 0.25, 0, 0.02, 0.5, 10);
	// Tuong trai
	block(-4.99, 0.25, 0, 0.02, 0.5, 10);
}
void veSanCongTruong() {
	Paint(244, 164, 96);
	block(0, -0.015, 0, 10.0, 0.03, 10.0);
	Paint(176, 196, 222);
	veAllTuong();
}

// Nha dang xay
void cotCai() {
	block(0.9, 0.55, -0.45, 0.08, 1.1, 0.08);
	block(-0.9, 0.55, 0.45, 0.08, 1.1, 0.08);
	block(0.9, 0.55, 0.45, 0.08, 1.1, 0.08);
	block(-0.9, 0.55, -0.45, 0.08, 1.1, 0.08);
}
void cotCon() {
	block(0.9, 0.55, -0.9, 0.05, 1.1, 0.05);
	block(-0.9, 0.55, 0.9, 0.05, 1.1, 0.05);
	block(0.9, 0.55, 0.9, 0.05, 1.1, 0.05);
	block(-0.9, 0.55, -0.9, 0.05, 1.1, 0.05);
	block(0, 0.55, -0.9, 0.05, 1.1, 0.05);
	block(0, 0.55, 0.9, 0.05, 1.1, 0.05);
}
void tangNha() {
	block(0, 0.13, 0, 2, 0.02, 2);
	block(0, 0.26, 0, 2, 0.02, 2);
	block(0, 0.4, 0, 2, 0.02, 2);
	block(0, 0.55, 0, 2, 0.02, 2);
	block(0, 0.7, 0, 2, 0.02, 2);
	block(0, 0.85, 0, 2, 0.02, 2);
	block(0, 1, 0, 2, 0.02, 2);
}
void nhaDangXay() {
	Paint(128, 128, 128);
	cotCai();
	cotCon();
	Paint(255, 255, 0);
	tangNha();
}

// Xe tai
GLfloat rtdY = 0, rtTXX = 0;
void cabinXeTai() {
	Paint(0, 255, 0);
	mat4 mdl = model;

	// Front + back cabin
	block(0, 0.24, 0.48, 0.85, 0.4, 0.04);
	block(0.5, 0.5, 0.48, 0.15, 0.92, 0.04);
	block(-0.5, 0.5, 0.48, 0.15, 0.92, 0.04);
	block(0, 0.91, 0.48, 0.85, 0.1, 0.04);

	block(0, 0.5, -0.48, 1.15, 0.92, 0.04);

	// Top + bottom cabin
	block(0, 0.98, 0, 1.15, 0.04, 1);

	block(0, 0.02, 0, 1.15, 0.04, 1);

	// Door left
	model = model * Translate(0.555, 0, 0.46) * RotateY(rtdY) * Translate(0, 0, -0.46);
	block(0, 0.24, 0, 0.04, 0.4, 0.62);
	block(0, 0.5, 0.385, 0.04, 0.92, 0.15);
	block(0, 0.5, -0.385, 0.04, 0.92, 0.15);
	block(0, 0.91, 0, 0.04, 0.1, 0.62);

	// Door right
	model = mdl * Translate(-0.555, 0, 0.46) * RotateY(-rtdY) * Translate(0, 0, -0.46);
	block(0, 0.24, 0, 0.04, 0.4, 0.62);
	block(0, 0.5, 0.385, 0.04, 0.92, 0.15);
	block(0, 0.5, -0.385, 0.04, 0.92, 0.15);
	block(0, 0.91, 0, 0.04, 0.1, 0.62);
}
void thungXeTai() {
	// Be thung
	block(0, 0.02, -1.65, 1.15, 0.04, 2.3);

	// Day Thung
	model = model * Translate(0, 0.04, -2.8) * RotateX(rtTXX) * Translate(0, 0, 2.8);
	// Day
	block(0, 0.02, -1.675, 1.15, 0.04, 2.25);
	// Thanh truoc
	block(0, 0.50, -0.57, 1.15, 0.92, 0.04);
	// Thanh trai + phai
	block(0.555, 0.50, -1.675, 0.04, 0.92, 2.17);
	block(-0.555, 0.50, -1.675, 0.04, 0.92, 2.17);

	// Cua thung
	model = model * Translate(0, 0.92, -2.76) * RotateX(-rtTXX) * Translate(0, 0, 2.76);
	block(0, -0.42, -2.78, 1.15, 0.92, 0.04);
}
void allWheel() {
	wheel(0.5, 0, 0, 0.15, 0.25);
	wheel(-0.5, 0, 0, 0.15, 0.25);
	wheel(0.5, 0, -2.25, 0.15, 0.25);
	wheel(-0.5, 0, -2.25, 0.15, 0.25);
}
void xeTai() {
	mat4 mdlXT = model;

	model = mdlXT * Translate(0, 0.005, 0);
	cabinXeTai();

	model = mdlXT * Translate(0, 0.005, 0);
	thungXeTai();

	model = mdlXT;
	allWheel();
}

void cabinXeTai1() {
	Paint(0, 255, 0);
	mat4 mdl = model;

	// Front + back cabin
	block(0, 0.24, 0.48, 0.85, 0.4, 0.04);
	block(0.5, 0.5, 0.48, 0.15, 0.92, 0.04);
	block(-0.5, 0.5, 0.48, 0.15, 0.92, 0.04);
	block(0, 0.91, 0.48, 0.85, 0.1, 0.04);

	block(0, 0.5, -0.48, 1.15, 0.92, 0.04);

	// Top + bottom cabin
	block(0, 0.98, 0, 1.15, 0.04, 1);

	block(0, 0.02, 0, 1.15, 0.04, 1);

	// Door left
	model = model * Translate(0.555, 0, 0.46) * RotateY(rtdY) * Translate(0, 0, -0.46);
	block(0, 0.24, 0, 0.04, 0.4, 0.62);
	block(0, 0.5, 0.385, 0.04, 0.92, 0.15);
	block(0, 0.5, -0.385, 0.04, 0.92, 0.15);
	block(0, 0.91, 0, 0.04, 0.1, 0.62);

	// Door right
	model = mdl * Translate(-0.555, 0, 0.46) * RotateY(-rtdY) * Translate(0, 0, -0.46);
	block(0, 0.24, 0, 0.04, 0.4, 0.62);
	block(0, 0.5, 0.385, 0.04, 0.92, 0.15);
	block(0, 0.5, -0.385, 0.04, 0.92, 0.15);
	block(0, 0.91, 0, 0.04, 0.1, 0.62);
}
void thungXeTai1() {
	// Be thung
	block(0, 0.02, -1.65, 1.15, 0.04, 2.3);

	// Day Thung
	model = model * Translate(0, 0.04, -2.8) * RotateX(rtTXX) * Translate(0, 0, 2.8);
	// Day
	block(0, 0.02, -1.675, 1.15, 0.04, 2.25);
	// Thanh truoc
	block(0, 0.50, -0.57, 1.15, 0.92, 0.04);
	// Thanh trai + phai
	block(0.555, 0.50, -1.675, 0.04, 0.92, 2.17);
	block(-0.555, 0.50, -1.675, 0.04, 0.92, 2.17);

	// Cua thung
	model = model * Translate(0, 0.92, -2.76) * RotateX(-rtTXX) * Translate(0, 0, 2.76);
	block(0, -0.42, -2.78, 1.15, 0.92, 0.04);
}
void allWheel1() {
	wheel(0.5, 0, 0, 0.15, 0.25);
	wheel(-0.5, 0, 0, 0.15, 0.25);
	wheel(0.5, 0, -2.25, 0.15, 0.25);
	wheel(-0.5, 0, -2.25, 0.15, 0.25);
}
void xeTai1() {
	mat4 mdlXT = model;

	model = mdlXT * Translate(0, 0.005, 0);
	cabinXeTai1();

	model = mdlXT * Translate(0, 0.005, 0);
	thungXeTai1();

	model = mdlXT;
	allWheel1();
}

// Can cau
GLfloat tdCBCC = 0.0, rtCCY = 0.0, tYHHCC = 0.25, tZHHCC = 2.175;
void cotCanCau() {
	block(0, 3.3, 0, 0.25, 6.6, 0.25);

	for (GLfloat i = 1.1 / 3; i < 6.6; i += 1.1 / 3)
		block(0, i, 0.125, 0.25, 0.1, 0.1);
}
void trucVaTaCanCau() {
	block(0.15, 6.625, 1.1, 0.05, 0.05, 6.6);
	block(0, 6.625, 1.1, 0.00275, 0.0275, 6.6);
	block(-0.15, 6.625, 1.1, 0.05, 0.05, 6.6);

	// Cac thanh xa
	for (GLfloat i = 0.525; i < 4.4; i += 0.55) {
		block(0, 6.625, i, 0.25, 0.05, 0.05);
		if (-i > -2.2)
			block(0, 6.625, -i, 0.25, 0.05, 0.05);
	}

	// Ta can cau
	block(0, 6.625, -1.9, 0.25, 0.5, 0.5);
}
void cabinCanCau() {
	mat4 mdlCBCC = model;
	block(0, 6.65, 0.2625, 0.3, 0.025, 0.025);
	block(0, 6.8375, 0.2625, 0.3, 0.025, 0.025);
	block(0.1625, 6.75, 0.2625, 0.025, 0.2, 0.025);
	block(-0.1625, 6.75, 0.2625, 0.025, 0.2, 0.025);

	block(0, 6.65, -0.2625, 0.3, 0.025, 0.025);
	block(0, 6.8375, -0.2625, 0.3, 0.025, 0.025);
	block(0.1625, 6.75, -0.2625, 0.025, 0.2, 0.025);
	block(-0.1625, 6.75, -0.2625, 0.025, 0.2, 0.025);

	block(0.1625, 6.65, 0, 0.025, 0.025, 0.5);
	block(0.1625, 6.8375, 0, 0.025, 0.025, 0.5);
	block(0.1625, 6.75, 0.26, 0.025, 0.2, 0.025);
	block(0.1625, 6.75, 0.26, 0.025, 0.2, 0.025);

	block(-0.1625, 6.65, 0, 0.025, 0.025, 0.5);
	block(-0.1625, 6.8375, 0, 0.025, 0.025, 0.5);
	block(-0.1625, 6.75, -0.26, 0.025, 0.2, 0.025);
	block(-0.1625, 6.75, -0.26, 0.025, 0.2, 0.025);

	block(0, 6.8375, 0, 0.35, 0.025, 0.5);
	block(0, 6.625, 0.20625, 0.25, 0.05, 0.1375);
	block(0, 6.625, -0.20625, 0.25, 0.05, 0.1375);

	model = model * Translate(0, 0, tdCBCC);
	block(0, 6.625, 0.06875, 0.25, 0.05, 0.1375);
	model = mdlCBCC * Translate(0, 0, -tdCBCC);
	block(0, 6.625, -0.06875, 0.25, 0.05, 0.1375);

}
void canCau() {
	Paint(255, 0, 0);
	cotCanCau();

	// Truc ngang va cabin can cau
	model = model * RotateY(rtCCY);
	mat4 mdlCC = model;
	trucVaTaCanCau();
	cabinCanCau();

	// Hang hoa
	model = mdlCC *
		Translate(0, (6.6 + tYHHCC) / 2, tZHHCC) *
		Scale(0.0275, 6.6 - tYHHCC, 0.0275);
	block(0, 0, 0, 1, 1, 1);
	model = mdlCC * Translate(0, tYHHCC, tZHHCC) * Scale(0.5);
	block(0, 0, 0, 1, 1, 1);
}

// Xe nang
GLfloat rtdXN = 0, tHXN = 0.025;
void allXNWheel() {
	wheel(0.2, 0, 0.2, 0.065, 0.1);
	wheel(-0.2, 0, 0.2, 0.065, 0.1);
	wheel(0, 0, -0.35, 0.065, 0.1);
}
void xeNang() {
	Paint(0, 0, 255);
	model = model * Translate(0, 0.065, 0);
	block(0, 0.01, 0, 0.5, 0.02, 0.5);
	block(0, 0.98, 0, 0.5, 0.02, 0.5);

	block(0, 0.26, 0.24, 0.3, 0.48, 0.02);
	block(0, 0.92, 0.24, 0.3, 0.1, 0.02);
	block(0.2, 0.5, 0.24, 0.1, 0.96, 0.02);
	block(-0.2, 0.5, 0.24, 0.1, 0.96, 0.02);

	block(0, 0.26, -0.24, 0.3, 0.48, 0.02);
	block(0, 0.92, -0.24, 0.3, 0.1, 0.02);
	block(0.2, 0.5, -0.24, 0.1, 0.96, 0.02);
	block(-0.2, 0.5, -0.24, 0.1, 0.96, 0.02);

	block(0, 0.25, -0.35, 0.5, 0.5, 0.2);

	mat4 mdlXN = model;
	model = model * Translate(0.24, 0, 0.24) * RotateY(rtdXN) * Translate(-0.24, 0, 0);
	block(0.24, 0.26, -0.24, 0.02, 0.48, 0.3);
	block(0.24, 0.93, -0.24, 0.02, 0.1, 0.3);
	block(0.24, 0.5, -0.04, 0.02, 0.96, 0.1);
	block(0.24, 0.5, -0.44, 0.02, 0.96, 0.1);

	model = mdlXN * Translate(-0.24, 0, 0.24) * RotateY(-rtdXN) * Translate(0.24, 0, 0);
	block(-0.24, 0.26, -0.24, 0.02, 0.48, 0.3);
	block(-0.24, 0.93, -0.24, 0.02, 0.1, 0.3);
	block(-0.24, 0.5, -0.04, 0.02, 0.96, 0.1);
	block(-0.24, 0.5, -0.44, 0.02, 0.96, 0.1);

	model = mdlXN;
	block(0.2, 0.625, 0.275, 0.1, 1.25, 0.05);
	block(-0.2, 0.625, 0.275, 0.1, 1.25, 0.05);
	model = model * Translate(0, tHXN, 0);
	block(0.2, 0, 0.55, 0.1, 0.05, 0.5);
	block(-0.2, 0, 0.55, 0.1, 0.05, 0.5);

	model = mdlXN;
	allXNWheel();
}

GLfloat tXT = 0, tXN = 0;
void congTruong() {
	model = RotateX(rtCTX) * RotateY(rtCTY) * RotateZ(rtCTZ);
	mat4 mdlCT = model;
	veSanCongTruong();

	model = mdlCT * Translate(-3.5 - tXT, 0.02, 0) * RotateY(-90) * Scale(0.4);
	xeTai();
	model = mdlCT * RotateY(60) * Translate(4, 0.02, 2 * tXT) * Scale(0.4);
	xeTai1();

	model = mdlCT * Translate(tXN, 0, 3.3) * RotateY(-90);
	xeNang();

	model = mdlCT * Translate(2.88, 0, 2) * RotateY(-40);
	canCau();

	model = mdlCT * Translate(0, -0.02, 0) * Scale(2, 5, 2);
	nhaDangXay();

	//model = mdlCT;
	//vatThe();
}

GLfloat eX = 0.0, eY = 0.0, eZ = 10.0;
GLfloat lt = -5, rt = 5, bt = -5, tp = 5, zNr = 2.5, zFr = 12;
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	congTruong();
	vec4 eye(eX, eY, eZ, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);
	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(lt, rt, bt, tp, zNr, zFr);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glutSwapBuffers();
}
void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
}
void keyboard(unsigned char key, int x, int y)
{
	// Thoat, reset
	switch (key) {
		case 033:
			exit(1);
			break;
		case ' ':
			rtCTX = 0, rtCTY = 0, rtCTZ = 0;
			eX = 0, eY = 0.0, eZ = 10.0;
			lt = -5, rt = 5, bt = -5, tp = 5; 
			zNr = 2.5, zFr = 15;

			rtdY = 0, rtTXX = 0;
			rtdXN = 0, tHXN = 0.025;
			tdCBCC = 0.0, rtCCY = 0.0, tYHHCC = 0.25, tZHHCC = 2.175;
			break;
	}

	/* 
	* Dieu khien goc nhin va Cong Truong
	* 
	* Xoay toàn bộ công trường
	*   w  -     X      +  s
	*   a  -     Y      +  d
	*   e  -     Z      +  q
	*
	* Điều khiển vị trí camera
	*   l  -    eyeX    +  '
	*	;  -    eyeY    +  p
	*   [  -    eyeZ    +  o
	* 
	* Thay đổi viewing
	*	2  -     lt     +  1
	*	4  -     bt     +  3
	*   6  -  zNr, zFr  +  5
	*/
	switch (key) {
		case 's':
			rtCTX += 5;
			if (rtCTX > 360) rtCTX -= 360;
			break;
		case 'w':
			rtCTX -= 5;
			if (rtCTX < -360) rtCTX += 360;
			break;
		case 'd':
			rtCTY += 5;
			if (rtCTY > 360) rtCTY -= 360;
			break;
		case 'a':
			rtCTY -= 5;
			if (rtCTY < -360) rtCTY += 360;
			break;
		case 'q':
			rtCTZ += 5;
			if (rtCTZ > 360) rtCTZ -= 360;
			break;
		case 'e':
			rtCTZ -= 5;
			if (rtCTZ < -360) rtCTZ += 360;
			break;

		// Dieu khien camera, view
		case 0x27: eX += 0.1; break;
		case 'l': eX -= 0.1; break;
		case 'p': eY += 0.1; break;
		case ';': eY -= 0.1; break;
		case 'o': eZ += 0.1; break;
		case '[': eZ -= 0.1; break;

		case '1': lt *= 1.1; rt *= 1.1; break;
		case '2': lt *= 0.9; rt *= 0.9; break;
		case '3': bt *= 1.1; tp *= 1.1; break;
		case '4': bt *= 0.9; tp *= 0.9; break;
		case '5': zNr *= 1.1; zFr *= 1.1; break;
		case '6': zNr *= 0.9; zFr *= 0.9; break;

		//case '&': rs *= 2.0; break;
		//case '*': rs *= 0.5; break;
		//case '(': tt += dr; break;
		//case ')': tt -= dr; break;
		//case '_': ph += dr; break;
		//case '+': ph -= dr; break;
	}

	/*
	* Dieu khien xe tai:
	* r, R: tien, lui
	* Z, z: dong, mo cua
	* X, x: nang, ha thung xe
	*/
	switch (key) {
		// Tien lui
		case 'r':
			tXT += 0.1;
			if (tXT > 1) tXT = 1;
			break;
		case 'R':
			tXT -= 0.1;
			if (tXT < 0) tXT = 0;
			break;

		// Mo, dong cua
		case 'Z':
			rtdY += 5;
			if (rtdY > 0) rtdY = 0;
			break;
		case 'z':
			rtdY -= 5;
			if (rtdY < -90) rtdY = -90;
			break;

		// Nang, ha thung xe
		case 'X':
			rtTXX += 2;
			if (rtTXX > 0) rtTXX = 0;
			break;
		case 'x':
			rtTXX -= 2;
			if (rtTXX < -40) rtTXX = -40;
			break;
	}

	/*
	* Dieu khien can cau:
	* t, T: nang, ha hang hoa
	* y, Y: tien, lui hang hoa
	* C, c: xoay can cau
	* v, V: mo cua cabin
	*/
	switch (key) {
		// Nang, ha hang hoa
		case 't':
			tYHHCC += 0.1;
			if (tYHHCC > 6.325) tYHHCC = 6.325;
			break;
		case 'T':
			tYHHCC -= 0.1;
			if (tYHHCC < 0.25) tYHHCC = 0.25;
			break;

		// Tien, lui hang hoa
		case 'y':
			tZHHCC += 0.1;
			if (tZHHCC > 4.375) tZHHCC = 4.375;
			break;;
		case 'Y':
			tZHHCC -= 0.1;
			if (tZHHCC < 0.55) tZHHCC = 0.55;
			break;

		// Xoay can cau
		case 'C':
			rtCCY += 5;
			if (rtCCY > 360) rtCCY -= 360;
			break;
		case 'c':
			rtCCY -= 5;
			if (rtCCY < -360) rtCCY += 360;
			break;

		// Mo cua cabin
		case 'v':
			tdCBCC += 0.005;
			if (tdCBCC > 0.1375) tdCBCC = 0.1375;
			break;
		case 'V':
			tdCBCC -= 0.005;
			if (tdCBCC < 0.0) tdCBCC = 0.0;
			break;
	}

	/*
	* Dieu khien xe nang
	* i, I: tien, lui
	* , <: Mo, dong cua
	* . >: Nang, ha cang xe
	*/
	switch (key) {
		// Tien lui
		case 'i':
			tXN -= 0.1;
			if (tXN < -1.5) tXN = -1.5;
			break;
		case 'I':
			tXN += 0.1;
			if (tXN > 0) tXN = 0;
			break;

		// Mo, dong cua
		case ',':
			rtdXN -= 5;
			if (rtdXN < -90) rtdXN = -90;
			break;
		case '<':
			rtdXN += 5;
			if (rtdXN > 0) rtdXN = 0;
			break;

		// Nang, ha cang xe
		case '.':
			tHXN += 0.025;
			if (tHXN > 1.225) tHXN = 1.225;
			break;
		case '>':
			tHXN -= 0.025;
			if (tHXN < 0.025) tHXN = 0.025;
			break;
	}

	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	// main function: program starts here

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(1000, 100);
	glutCreateWindow("Drawing a Cube");

	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
