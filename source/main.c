#include <sf_graphics.h>

#include <stdlib.h>

int
main(void)
{
	struct sf_arena arena	= {0};
	struct sf_graphics_glfw_platform *platform = NULL;
	struct sf_string title = {0};
	struct sf_graphics_renderer_description description = {0};
	struct sf_graphics_renderer *renderer = NULL;

	arena.alignment = 16;
	arena.capacity	= 1024 * 1024 * 16;
	arena.data	= malloc(1024 * 1024);
	if (!arena.data)
		return 0;


	title.data = "sf_graphics test";
	title.size = sizeof("sf_graphics test");
	platform = sf_graphics_create_glfw_platform(&arena, 800, 600, &title);
	if (!platform)
		goto error;

	sf_graphics_glfw_platform_fill_renderer_description(&arena, platform, &description);
	renderer = sf_graphics_create_renderer(&arena, &description);
	if (!renderer)
		goto error;

	while (!sf_graphics_glfw_platform_should_close(platform))
	{
		sf_graphics_glfw_platform_process_events(platform);
	}

error:
	sf_graphics_destroy_renderer(renderer);
	sf_graphics_destroy_glfw_platform(platform);
	free(arena.data);
}
