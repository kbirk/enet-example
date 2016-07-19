#include "gl/UniformBlockDescriptor.h"

UniformBlockDescriptor::Shared UniformBlockDescriptor::alloc(
	const std::string& blockName,
	const GLint blockIndex,
	const GLint blockSize,
	const std::map<std::string, GLint>& offsets) {
	return std::make_shared<UniformBlockDescriptor>(
		blockName,
		blockIndex,
		blockSize,
		offsets);
}

UniformBlockDescriptor::UniformBlockDescriptor(
	const std::string& blockName,
	const GLint blockIndex,
	const GLint blockSize,
	const std::map<std::string, GLint>& offsets)
	: blockName_(blockName)
	, blockIndex_(blockIndex)
	, blockSize_(blockSize)
	, offsets_(offsets) {}

GLuint UniformBlockDescriptor::blockIndex() const {
	return blockIndex_;
}

GLint UniformBlockDescriptor::alignedBlockSize() const {
	static GLint uniformBufferAlignment = -1;
	if (uniformBufferAlignment == -1) {
		// only ever need to query for this once, this is a global value, not a
		// per-program or per-block one
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignment);
		LOG_OPENGL("glGetIntegerv");
	}
	return blockSize_ + uniformBufferAlignment - (blockSize_ % uniformBufferAlignment);
}

GLint UniformBlockDescriptor::unAlignedBlockSize() const {
	return blockSize_;
}

GLint UniformBlockDescriptor::offset(const std::string& name) const {
	const auto& iter = offsets_.find(name);
	if (iter == offsets_.end()) {
		LOG_WARN("Uniform name does not match any offset descriptor, returning 0");
		return 0;
	}
	return iter->second;
}
