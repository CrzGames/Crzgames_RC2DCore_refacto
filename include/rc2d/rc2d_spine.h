#ifndef RC2D_SPINE_H
#define RC2D_SPINE_H

#include <spine/spine.h>
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

_SP_ARRAY_DECLARE_TYPE(spSdlVertexArray, struct SDL_Vertex)

typedef struct spSkeletonDrawable {
	spSkeleton *skeleton;
	spAnimationState *animationState;

	spSkeletonClipping *clipper;
	spFloatArray *worldVertices;
	spSdlVertexArray *sdlVertices;
	spIntArray *sdlIndices;
} spSkeletonDrawable;

SP_API spSkeletonDrawable *spSkeletonDrawable_create(spSkeletonData *skeletonData, spAnimationStateData *animationStateData);

SP_API void spSkeletonDrawable_dispose(spSkeletonDrawable *self);

SP_API void spSkeletonDrawable_update(spSkeletonDrawable *self, float delta);

SP_API void spSkeletonDrawable_draw(spSkeletonDrawable *self, struct SDL_Renderer *renderer);

#ifdef __cplusplus
}
#endif

#endif // RC2D_SPINE_H