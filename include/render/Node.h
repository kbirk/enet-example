#pragma once

#include "Common.h"
#include "render/Mesh.h"
#include "render/Transform.h"

#include <memory>
#include <vector>

class Node : public Transform, public std::enable_shared_from_this<Node> {

	public:

		typedef std::shared_ptr<Node> Shared;
		static Shared alloc(const std::string&);

		Node(const std::string&);

		void addChild(Node::Shared);
		void removeChild(Node::Shared);
		void clearChildren();

		void addMesh(Mesh::Shared);
		void removeMesh(Mesh::Shared);
		void clearMeshes();

		const std::string& name() const;
		const std::vector<Node::Shared>& children() const;
		const std::vector<Mesh::Shared>& meshes() const;

		glm::mat4 globalMatrix() const;
		glm::mat4 globalViewMatrix(bool = false) const;

	private:

		// prevent copy-construction
		Node(const Node&);
		// prevent assignment
		Node& operator= (const Node&);

		std::string name_;
		Node::Shared parent_;
		std::vector<Node::Shared> children_;
		std::vector<Mesh::Shared> meshes_;
};
