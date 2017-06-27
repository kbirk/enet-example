#include "render/Node.h"

#include <algorithm>

Node::Shared Node::alloc(const std::string& name)
{
    return std::make_shared<Node>(name);
}

Node::Node(const std::string& name)
    : Transform()
    , name_(name)
{
}

void Node::addChild(Node::Shared child)
{
    children_.push_back(child);
    child->parent_ = shared_from_this();
}

void Node::removeChild(Node::Shared child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
        child->parent_ = nullptr;
    }
}

void Node::clearChildren()
{
    children_ = std::vector<Node::Shared>();
}

void Node::addMesh(Mesh::Shared mesh)
{
    meshes_.push_back(mesh);
}

void Node::removeMesh(Mesh::Shared child)
{
    auto it = std::find(meshes_.begin(), meshes_.end(), child);
    if (it != meshes_.end()) {
        meshes_.erase(it);
    }
}

void Node::clearMeshes()
{
    meshes_ = std::vector<Mesh::Shared>();
}

const std::string& Node::name() const
{
    return name_;
}

const std::vector<Node::Shared>& Node::children() const
{
    return children_;
}

const std::vector<Mesh::Shared>& Node::meshes() const
{
    return meshes_;
}

glm::mat4 Node::globalMatrix() const
{
    if (parent_) {
        return parent_->globalMatrix() * matrix();
    }
    return matrix();
}

glm::mat4 Node::globalViewMatrix(bool ignoreTranslation) const
{
    if (parent_) {
        return parent_->globalMatrix() * viewMatrix(ignoreTranslation);
    }
    return viewMatrix(ignoreTranslation);
}
