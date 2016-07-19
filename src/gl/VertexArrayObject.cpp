#include "gl/VertexArrayObject.h"

VertexArrayObject::Shared VertexArrayObject::alloc() {
	return std::make_shared<VertexArrayObject>();
}

VertexArrayObject::VertexArrayObject()
	: id_(0) {
}

VertexArrayObject::~VertexArrayObject() {
	if (id_ != 0) {
		glDeleteVertexArrays(1, &id_);
		LOG_OPENGL("glDeleteVertexArrays");
		id_ = 0;
	}
}

void VertexArrayObject::attach(VertexBufferObject::Shared vbo, VertexAttributePointer::Shared pointer) {
	vbos_.push_back(vbo);
	pointers_.push_back(pointer);
}

void VertexArrayObject::attach(ElementArrayBufferObject::Shared eabo) {
	eabo_ = eabo;
}

void VertexArrayObject::draw() const {
	// bind vertex array object
	glBindVertexArray(id_);
	LOG_OPENGL("glBindVertexArray");
	// draw
	if (eabo_) {
		eabo_->draw();
	} else {
		for (auto& vbo : vbos_) {
			vbo->draw();
		}
	}
	// unbind vao
	glBindVertexArray(0);
	LOG_OPENGL("glBindVertexArray");
}

void VertexArrayObject::upload() {
	// create VAO
	if (!id_) {
		glGenVertexArrays(1, &id_);
		LOG_OPENGL("glGenVertexArrays");
	}
	// bind VAO
	glBindVertexArray(id_);
	LOG_OPENGL("glBindVertexArray");
	// bind all VBOs
	for (uint i=0; i<vbos_.size(); i++) {
		vbos_[i]->bind();
		pointers_[i]->bind();
	}
	// bind EABO
	if (eabo_) {
		eabo_->bind();
	}
	// unbind
	glBindVertexArray(0);
	LOG_OPENGL("glBindVertexArray");
}
