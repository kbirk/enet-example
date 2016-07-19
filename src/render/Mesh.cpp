#include "render/Mesh.h"

Mesh::Shared Mesh::alloc(
	VertexArrayObject::Shared vao,
	Material::Shared material,
	Technique::Shared technique) {
	return std::make_shared<Mesh>(vao, material, technique);
}

Mesh::Mesh(
	VertexArrayObject::Shared vao,
	Material::Shared material,
	Technique::Shared technique)
	: vao_(vao)
	, material_(material)
	, technique_(technique) {
}

RenderCommand::Shared Mesh::render() const {
	auto command = RenderCommand::alloc();
	command->merge(technique_->render());
	command->merge(material_->render());
	command->vao = vao_;
	return command;
}
