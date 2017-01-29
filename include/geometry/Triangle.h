#pragma once

#include "Common.h"
#include "geometry/Intersection.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Triangle {

	public:

		typedef std::shared_ptr<Triangle> Shared;
		static Shared alloc();
		static Shared alloc(
			const glm::vec3& normal,
			const glm::vec3& a,
			const glm::vec3& b,
			const glm::vec3& c);
		static Shared alloc(
			const glm::vec3&,
			const glm::vec3&,
			const glm::vec3&);

		Triangle();
		Triangle(
			const glm::vec3& normal,
			const glm::vec3& a,
			const glm::vec3& b,
			const glm::vec3& c);
		Triangle(
			const glm::vec3&,
			const glm::vec3&,
			const glm::vec3&);

		const glm::vec3& a() const;
		const glm::vec3& b() const;
		const glm::vec3& c() const;
		const glm::vec3& normal() const;
		const glm::vec3& centroid() const;
		float32_t radius() const;

		Intersection intersect(
			const glm::vec3&,
			const glm::vec3&,
			bool ignoreBehindRay = false,
			bool backFaceCull = false) const;

		bool contains(const glm::vec3&) const;
		glm::vec3 closestPointTo(const glm::vec3& point) const;
		glm::vec3 closestPointOnEdge(uint32_t edgeNum, const glm::vec3& point) const;

		friend bool operator== (const Triangle::Shared&, const Triangle::Shared&);
		friend bool operator!= (const Triangle::Shared&, const Triangle::Shared&);

	private:

		// prevent copy-construction
		Triangle(const Triangle&);
		// prevent assignment
		Triangle& operator= (const Triangle&);

		glm::vec3 a_;
		glm::vec3 b_;
		glm::vec3 c_;
		glm::vec3 normal_;
		glm::vec3 centroid_;
		float32_t radius_;

		void recalculate();
		void calcRadius();
};
