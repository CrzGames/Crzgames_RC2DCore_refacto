#include "rc2d/rc2d_spine.h"
#include "rc2d/rc2d_logger.h"

#include <spine/spine.h>
#include <spine/extension.h>
#include <SDL_image.h> // Required for : IMG_Load
#include <SDL.h> 

_SP_ARRAY_IMPLEMENT_TYPE_NO_CONTAINS(spSdlVertexArray, SDL_Vertex)

spSkeletonDrawable *spSkeletonDrawable_create(spSkeletonData *skeletonData, spAnimationStateData *animationStateData) {
    spBone_setYDown(-1);
    spSkeletonDrawable *self = NEW(spSkeletonDrawable);
    self->skeleton = spSkeleton_create(skeletonData);
    self->animationState = spAnimationState_create(animationStateData);
    self->sdlIndices = spIntArray_create(12);
    self->sdlVertices = spSdlVertexArray_create(12);
    self->worldVertices = spFloatArray_create(12);
    self->clipper = spSkeletonClipping_create();
    return self;
}

void spSkeletonDrawable_dispose(spSkeletonDrawable *self) {
    spSkeleton_dispose(self->skeleton);
    spAnimationState_dispose(self->animationState);
    spIntArray_dispose(self->sdlIndices);
    spSdlVertexArray_dispose(self->sdlVertices);
    spFloatArray_dispose(self->worldVertices);
    spSkeletonClipping_dispose(self->clipper);
    FREE(self);
}

void spSkeletonDrawable_update(spSkeletonDrawable *self, float delta) {
    spAnimationState_update(self->animationState, delta);
    spAnimationState_apply(self->animationState, self->skeleton);
    spSkeleton_updateWorldTransform(self->skeleton);
}

void spSkeletonDrawable_draw(spSkeletonDrawable *self, struct SDL_Renderer *renderer) {
    static unsigned short quadIndices[] = {0, 1, 2, 2, 3, 0};
    spSkeleton *skeleton = self->skeleton;
    spSkeletonClipping *clipper = self->clipper;
    SDL_Texture *texture;
    SDL_Vertex sdlVertex;
    
    for (int i = 0; i < skeleton->slotsCount; ++i) {
        spSlot *slot = skeleton->drawOrder[i];
        spAttachment *attachment = slot->attachment;
        if (!attachment) {
            spSkeletonClipping_clipEnd(clipper, slot);
            continue;
        }

        // Early out if the slot color is 0 or the bone is not active
        if (slot->color.a == 0 || !slot->bone->active) {
            spSkeletonClipping_clipEnd(clipper, slot);
            continue;
        }

        spFloatArray *vertices = self->worldVertices;
        int verticesCount = 0;
        float *uvs = NULL;
        unsigned short *indices;
        int indicesCount = 0;
        spColor *attachmentColor = NULL;

        if (attachment->type == SP_ATTACHMENT_REGION) {
            spRegionAttachment *region = (spRegionAttachment *) attachment;
            attachmentColor = &region->color;

            // Early out if the slot color is 0
            if (attachmentColor->a == 0) {
                spSkeletonClipping_clipEnd(clipper, slot);
                continue;
            }

            spFloatArray_setSize(vertices, 8);
            spRegionAttachment_computeWorldVertices(region, slot, vertices->items, 0, 2);
            verticesCount = 4;
            uvs = region->uvs;
            indices = quadIndices;
            indicesCount = 6;
            texture = (SDL_Texture *) ((spAtlasRegion *) region->rendererObject)->page->rendererObject;
        } else if (attachment->type == SP_ATTACHMENT_MESH) {
            spMeshAttachment *mesh = (spMeshAttachment *) attachment;
            attachmentColor = &mesh->color;

            // Early out if the slot color is 0
            if (attachmentColor->a == 0) {
                spSkeletonClipping_clipEnd(clipper, slot);
                continue;
            }

            spFloatArray_setSize(vertices, mesh->super.worldVerticesLength);
            spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, vertices->items, 0, 2);
            verticesCount = mesh->super.worldVerticesLength >> 1;
            uvs = mesh->uvs;
            indices = mesh->triangles;
            indicesCount = mesh->trianglesCount;
            texture = (SDL_Texture *) ((spAtlasRegion *) mesh->rendererObject)->page->rendererObject;
        } else if (attachment->type == SP_ATTACHMENT_CLIPPING) {
            spClippingAttachment *clip = (spClippingAttachment *) slot->attachment;
            spSkeletonClipping_clipStart(clipper, slot, clip);
            continue;
        } else {
            continue;
        }

        Uint8 r = (Uint8) (skeleton->color.r * slot->color.r * attachmentColor->r * 255);
        Uint8 g = (Uint8) (skeleton->color.g * slot->color.g * attachmentColor->g * 255);
        Uint8 b = (Uint8) (skeleton->color.b * slot->color.b * attachmentColor->b * 255);
        Uint8 a = (Uint8) (skeleton->color.a * slot->color.a * attachmentColor->a * 255);
        sdlVertex.color.r = r;
        sdlVertex.color.g = g;
        sdlVertex.color.b = b;
        sdlVertex.color.a = a;

        if (spSkeletonClipping_isClipping(clipper)) {
            spSkeletonClipping_clipTriangles(clipper, vertices->items, verticesCount << 1, indices, indicesCount, uvs, 2);
            vertices = clipper->clippedVertices;
            verticesCount = clipper->clippedVertices->size >> 1;
            uvs = clipper->clippedUVs->items;
            indices = clipper->clippedTriangles->items;
            indicesCount = clipper->clippedTriangles->size;
        }

        spSdlVertexArray_clear(self->sdlVertices);
        for (int ii = 0; ii < verticesCount << 1; ii += 2) {
            sdlVertex.position.x = vertices->items[ii];
            sdlVertex.position.y = vertices->items[ii + 1];
            sdlVertex.tex_coord.x = uvs[ii];
            sdlVertex.tex_coord.y = uvs[ii + 1];
            spSdlVertexArray_add(self->sdlVertices, sdlVertex);
        }
        spIntArray_clear(self->sdlIndices);
        for (int ii = 0; ii < (int) indicesCount; ii++) {
            spIntArray_add(self->sdlIndices, indices[ii]);            
        }

        switch (slot->data->blendMode) {
            case SP_BLEND_MODE_NORMAL:
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                break;
            case SP_BLEND_MODE_MULTIPLY:
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
                break;
            case SP_BLEND_MODE_ADDITIVE:
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
                break;
            case SP_BLEND_MODE_SCREEN:
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                break;
        }

        SDL_RenderGeometry(renderer, texture, self->sdlVertices->items, self->sdlVertices->size, self->sdlIndices->items, indicesCount);
        spSkeletonClipping_clipEnd(clipper, slot);
    }
    
    spSkeletonClipping_clipEnd2(clipper);
}

// Ne pas modifier le nom de la fonction utiliser dans le runtime spine-c
void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
    SDL_RWops *rw = SDL_RWFromFile(path, "rb");
    if (!rw) {
        rc2d_log(RC2D_LOG_ERROR, "Erreur lors de l'ouverture du fichier %s\n", path);
        return;
    }

    SDL_Surface *surface = IMG_Load_RW(rw, 1); // 1 pour indiquer à SDL de libérer rw une fois fini
    if (!surface) {
        rc2d_log(RC2D_LOG_ERROR, "Impossible de charger l'image %s: %s\n", path, IMG_GetError());
        return;
    }

    // Créer la texture SDL avec les paramètres spécifiques
    SDL_Texture *texture = SDL_CreateTexture((SDL_Renderer *) self->atlas->rendererObject,
                                             SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, 
                                             surface->w, surface->h);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    // Mettre à jour la texture avec les données de la surface
    if (SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch)) {
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        return;
    }

    // Libérer la surface car elle n'est plus nécessaire
    SDL_FreeSurface(surface);

    // Assigner la texture au rendererObject
    self->rendererObject = texture;
}

// Ne pas modifier le nom de la fonction utiliser dans le runtime spine-c
void _spAtlasPage_disposeTexture(spAtlasPage *self) {
    SDL_DestroyTexture((SDL_Texture *) self->rendererObject);
}

// Ne pas modifier le nom de la fonction utiliser dans le runtime spine-c
char *_spUtil_readFile(const char *path, int *length) {
    return _spReadFile(path, length);
}