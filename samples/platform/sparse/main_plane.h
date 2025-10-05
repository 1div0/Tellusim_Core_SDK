/*
 */
constexpr uint32_t num_plane_vertices = 4 * 8;
constexpr uint32_t num_plane_indices = 2 * 3;

/*
 */
static const float32_t plane_vertices[num_plane_vertices] = {
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
};

static const uint32_t plane_indices[num_plane_indices] = {
	 0,  1,  2,  2,  3,  0,
};
