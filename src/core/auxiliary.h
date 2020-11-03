//
// Created by admin on 2020/11/2.
//

#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "scene.h"
#include "film.h"


namespace pbrt {

template<typename T>
using DualType = std::pair<T, T>;

struct DualBuffer;
struct DualBufferTile;
struct AuxiliaryBuffers;
struct AuxiliaryBuffersTile;
struct PerRayData;

struct DualBuffer {
    std::unique_ptr<Film> a, b, tot;  // A buffer, B buffer, total buffer
    std::unique_ptr<Film> a2, b2, tot2;  // A^2, B^2, total^2 (for variance estimation)
    int64_t spp_2; // spp / 2

    explicit DualBuffer(const std::string &name, int64_t spp);

    DualBufferTile GetFilmTile(const Bounds2i &sampleBounds);

    void MergeTile(DualBufferTile &dualTile);

    void WriteImage() const;
};

struct DualBufferTile {
    std::unique_ptr<FilmTile> a, b, tot;
    std::unique_ptr<FilmTile> a2, b2, tot2;
    int64_t spp_2;

    DualBufferTile() = delete;

    void AddSample(int64_t currentSampleIndex, const Point2f &pFilm, const Spectrum &L, Float sampleWeight = 1.);
};

struct AuxiliaryBuffers {
    DualBuffer radiance, albedo, normal, depth;

    explicit AuxiliaryBuffers(const std::string &prefix, int64_t spp);

    AuxiliaryBuffersTile GetFilmTile(const Bounds2i &sampleBounds);

    void MergeTile(AuxiliaryBuffersTile &tile);

    void WriteImage() const;
};

struct AuxiliaryBuffersTile {
    DualBufferTile radiance, albedo, normal, depth;

    AuxiliaryBuffersTile() = delete;

    void AddSample(int64_t currentSampleIndex, const Point2f &pFilm, const PerRayData &prd, Float sampleWeight = 1.);
};

struct PerRayData {
    Spectrum radiance, albedo;
    Normal3f normal;
    Float depth;
};

}
