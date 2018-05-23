#include "view.h"

Shader* View::viewShader = NULL;

View::View(float x, float y, float width, float height) : backgroundColor(0.0f, 0.0f, 0.0f, 0.0f), rotation(0.0f), scalar(1.0f, 1.0f, 1.0f), clipSubviews(false), clipToParent(false), isScrollable(false) {
	if (viewShader == NULL) {
		viewShader = new Shader("res/glsl/viewVertexShader.glsl", "res/glsl/viewFragmentShader.glsl");
	}
	shader = viewShader;
	verticies[0] = verticies[6] = x;
	verticies[1] = verticies[3] = y;
	verticies[2] = verticies[4] = x + width;
	verticies[5] = verticies[7] = y + height;
	unsigned int indicies[] = {
		0, 1, 2,
		0, 2, 3
	};
	glGenBuffers(2, vbo);
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	float textureCoordinates[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
}
/* fix later
View::View(const View& view) : vao(view.vao), backgroundColor(view.backgroundColor), rotation(view.rotation), translation(view.translation), scalar(view.scalar), subviews(view.subviews), clipSubviews(view.clipSubviews), clipToParent(view.clipToParent), isScrollable(view.isScrollable), offsetPosition(view.offsetPosition) {
	viewShader = view.viewShader;
	for (int i = 0; i < 8; i++) {
		verticies[i] = view.verticies[i];
	}
}
*/
View::~View() {
	for (std::list<View*>::iterator it = subviews.begin(); it != subviews.end(); it++) {
		delete *it;
	}
	// add buffer data deletion
}

void View::setBounds(float x, float y, float width, float height) {
	verticies[0] = verticies[6] = x;
	verticies[1] = verticies[3] = y;
	verticies[2] = verticies[4] = x + width;
	verticies[5] = verticies[7] = y + height;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticies), verticies);
}

Rectangle View::getBounds() const {
	return Rectangle(verticies[0], verticies[1], verticies[2] - verticies[0], verticies[5] - verticies[1]);
}

void View::setBackgroundColor(const Color& bc) {
	backgroundColor = bc;
}

void View::draw(float parentX, float parentY, float parentWidth, float parentHeight, const glm::mat4& parentModel) {
	Rectangle bounds = getBounds();
	glm::mat4 model;
	model = glm::translate(model, translation);
	model = glm::translate(model, glm::vec3(bounds.getX() + bounds.getWidth() / 2, bounds.getY() + bounds.getHeight() / 2, 0.0f));
	model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scalar);
	model = glm::translate(model, glm::vec3(-bounds.getX() - bounds.getWidth() / 2, -bounds.getY() - bounds.getHeight() / 2, 0.0f));
	float minX = parentX;
	float maxX = parentX + parentWidth;
	if (parentWidth < 0) {
		std::swap(minX, maxX);
	}
	float minY = parentY;
	float maxY = parentY + parentHeight;
	if (parentHeight < 0) {
		std::swap(minY, maxY);
	}
	float viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
	glm::mat4 projection = glm::ortho(0.0f, viewport[2], viewport[3], 0.0f, -0.1f, 0.1f);
	shader->use();
	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CLIP_DISTANCE1);
	glEnable(GL_CLIP_DISTANCE2);
	glEnable(GL_CLIP_DISTANCE3);
	shader->setUniform("clipRect", minX, minY, maxX, maxY);

	shader->setUniform("model", 1, false, glm::value_ptr(model));
	shader->setUniform("parentModel", 1, false, glm::value_ptr(parentModel));
	shader->setUniform("projection", 1, false, glm::value_ptr(projection));
	shader->setUniform("backgroundColor", backgroundColor.getRed(), backgroundColor.getGreen(), backgroundColor.getBlue(), backgroundColor.getAlpha());
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	shader->finish();
	for (std::list<View*>::iterator it = subviews.begin(); it != subviews.end(); it++) {
		View* view = *it;
		view->translate(bounds.getX() - offsetPosition.getX(), bounds.getY() - offsetPosition.getY());
		if (view->getClipToParent() || clipSubviews) {
		} else {
		}
		view->draw(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), parentModel * model);
		view->translate(offsetPosition.getX() - bounds.getX(), offsetPosition.getY() - bounds.getY());
	}
}

void View::addSubview(View* view) {
	subviews.push_back(view);
}

void View::setClipSubviews(bool clipSub) {
	clipSubviews = clipSub;
}

void View::setClipToParent(bool clipToP) {
	clipToParent = clipToP;
}

bool View::getClipToParent() const {
	return clipToParent;
}

void View::setIsScrollable(bool s) {
	isScrollable = s;
}

bool View::getIsScrollable() const {
	return isScrollable;
}

void View::scroll(double xOffset, double yOffset, float mouseX, float mouseY) {
	Rectangle bounds = getBounds();
	for (std::list<View*>::iterator it = subviews.begin(); it != subviews.end(); it++) {
		View* view = *it;
		Rectangle subBounds = view->getBounds();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(subBounds.getX() + subBounds.getWidth() / 2, subBounds.getY() + subBounds.getHeight() / 2, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f / view->scalar.x, 1.0f / view->scalar.y, 1.0f));
		model = glm::rotate(model, -view->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-subBounds.getX() - subBounds.getWidth() / 2, -subBounds.getY() - subBounds.getHeight() / 2, 0.0f));
		model = glm::translate(model, -view->translation);
		glm::vec3 pos = model * glm::vec4(mouseX - bounds.getX(), mouseY - bounds.getY(), 0.0f, 1.0f);
		if (subBounds.contains(Position(pos.x, pos.y))) {
			view->scroll(xOffset, yOffset, mouseX - bounds.getX(), mouseY - bounds.getY());
		}
	}
	if (isScrollable) {
		onScroll(xOffset, yOffset);
	}
}

void View::onScroll(double xOffset, double yOffset) {
}

void View::translate(float x, float y) {
	translation += glm::vec3(x, y, 0.0f);
}

void View::setTranslation(float x, float y) {
	translation = glm::vec3(x, y, 0.0f);
}

void View::rotate(float radians) {
	rotation += radians;
}

void View::setRotation(float radians) {
	rotation = radians;
}

void View::scale(float x, float y) {
	scalar *= glm::vec3(x, y, 0.0f);
}

void View::setScalar(float x, float y) {
	scalar = glm::vec3(x, y, 0.0f);
}

Position View::getOffsetPosition() const {
	return offsetPosition;
}

void View::setOffsetPosition(const Position& offset) {
	offsetPosition = offset;
}
