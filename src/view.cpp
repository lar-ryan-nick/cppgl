#include "view.h"

View::View(float x, float y, float width, float height) : shader("res/glsl/viewVertexShader.glsl", "res/glsl/viewFragmentShader.glsl"), backgroundColor(1.0f, 1.0f, 1.0f, 1.0f), clipSubviews(false), clipToParent(false), isScrollable(false) {
	verticies[0] = verticies[6] = x;
	verticies[1] = verticies[3] = y;
	verticies[2] = verticies[4] = x + width;
	verticies[5] = verticies[7] = y + height;
	unsigned int indicies[] = {
		0, 1, 2,
		0, 2, 3
	};
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

View::View(const View& view) : shader("res/glsl/viewVertexShader.glsl", "res/glsl/viewFragmentShader.glsl"), vao(view.vao), backgroundColor(view.backgroundColor), model(view.model), subviews(view.subviews), clipSubviews(view.clipSubviews), clipToParent(view.clipToParent), isScrollable(view.isScrollable), offsetPosition(view.offsetPosition) {
	for (int i = 0; i < 8; i++) {
		verticies[i] = view.verticies[i];
	}
}

View::~View() {
	for (std::list<View*>::iterator it = subviews.begin(); it != subviews.end(); it++) {
		delete *it;
	}
}

void View::setBounds(float x, float y, float width, float height) {
	verticies[0] = verticies[6] = x;
	verticies[1] = verticies[3] = y;
	verticies[2] = verticies[4] = x + width;
	verticies[5] = verticies[7] = y + height;
}

Rectangle View::getBounds() const {
	return Rectangle(verticies[0], verticies[1], verticies[2] - verticies[0], verticies[5] - verticies[1]);
}

void View::setBackgroundColor(const Color& bc) {
	backgroundColor = bc;
}

void View::draw(float parentX, float parentY, float parentWidth, float parentHeight) {
	shader.use();
	shader.setUniform("model", 1, false, glm::value_ptr(model));
	glm::mat4 projection = glm::ortho(-parentX, parentWidth - parentX, parentHeight - parentY, -parentY, -0.1f, 0.1f);
	shader.setUniform("projection", 1, false, glm::value_ptr(projection));
	shader.setUniform("backgroundColor", backgroundColor.getRed(), backgroundColor.getGreen(), backgroundColor.getBlue(), backgroundColor.getAlpha());
	Rectangle bounds = getBounds();
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	for (std::list<View*>::iterator it = subviews.begin(); it != subviews.end(); it++) {
		View* view = *it;
		if (view->getClipToParent() || clipSubviews) {
			float viewport[4];
			glGetFloatv(GL_VIEWPORT, viewport);
			float viewWidth = bounds.getWidth();
			if (parentX + bounds.getX() + viewWidth > parentWidth) {
				viewWidth -= parentWidth;
				if (viewWidth < 0) {
					viewWidth = 0;
				}
			}
			float viewHeight = bounds.getHeight();
			if (parentY + bounds.getY() + viewHeight > parentHeight) {
				viewHeight -= parentHeight;
				if (viewHeight < 0) {
					viewHeight = 0;
				}
			}
			glViewport(parentX + bounds.getX(), parentHeight - bounds.getY() - bounds.getHeight(), viewWidth, viewHeight);
			view->draw(-offsetPosition.getX(), -offsetPosition.getY(), viewWidth, viewHeight);
			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		} else {
			view->draw(bounds.getX() + parentX, bounds.getY() + parentY, parentWidth, parentHeight);
		}
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

void View::scroll(double xOffset, double yOffset) {
	for (std::list<View*>::iterator it = subviews.begin(); it != subviews.end(); it++) {
		View* view = *it;
		view->scroll(xOffset, yOffset);
	}
	if (isScrollable) {
		onScroll(xOffset, yOffset);
	}
}

void View::onScroll(double xOffset, double yOffset) {
}

Position View::getOffsetPosition() const {
	return offsetPosition;
}

void View::setOffsetPosition(const Position& offset) {
	offsetPosition = offset;
}
