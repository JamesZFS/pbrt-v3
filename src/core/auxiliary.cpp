//
// Created by admin on 2020/11/2.
//

#include "auxiliary.h"
#include "api.h"

namespace pbrt {

DualBuffer::DualBuffer(const std::string &name, int64_t spp) :
        a{MakeBufferLikeFilm(name + "-A.exr")}, b{MakeBufferLikeFilm(name + "-B.exr")},
        tot{MakeBufferLikeFilm(name + "-tot.exr")},
        a2{MakeBufferLikeFilm(name + "-A_var.exr")}, b2{MakeBufferLikeFilm(name + "-B_var.exr")},
        tot2{MakeBufferLikeFilm(name + "-tot_var.exr")},
        spp_2(spp / 2) {}

DualBufferTile DualBuffer::GetFilmTile(const Bounds2i &sampleBounds) {
    return DualBufferTile{
            a->GetFilmTile(sampleBounds), b->GetFilmTile(sampleBounds), tot->GetFilmTile(sampleBounds),
            a2->GetFilmTile(sampleBounds), b2->GetFilmTile(sampleBounds), tot2->GetFilmTile(sampleBounds),
            spp_2
    };
}

void DualBuffer::MergeTile(DualBufferTile dualTile) {
    a->MergeFilmTile(std::move(dualTile.a)), b->MergeFilmTile(std::move(dualTile.b)), tot->MergeFilmTile(
            std::move(dualTile.tot));
    a2->MergeFilmTile(std::move(dualTile.a2)), b2->MergeFilmTile(std::move(dualTile.b2)), tot2->MergeFilmTile(
            std::move(dualTile.tot2));
}

void DualBuffer::WriteImage() const {
    a->WriteImage(1, true), b->WriteImage(1, true), tot->WriteImage(1, true);
    // Now that xyz buffer of a, b, and tot stores the final mean value
    auto makeOp = [](Float n, const std::unique_ptr<Film> &EX) {
        Float w = (Float) 1 / (n - 1);
        return [w, &EX](Float *rgb, const Point2i &p) {
            Float *ex = EX->GetPixel(p).xyz;
            // Var(X) = (EX^2 - (EX)^2) * n/(n-1)
            rgb[0] = (rgb[0] - ex[0] * ex[0]) * w;
            rgb[1] = (rgb[1] - ex[1] * ex[1]) * w;
            rgb[2] = (rgb[2] - ex[2] * ex[2]) * w;
        };
    };
    // Write image with post-processing to get variance
    a2->WriteImage(1, false, makeOp(spp_2, a)), b2->WriteImage(1, false, makeOp(spp_2, b));
    tot2->WriteImage(1, false, makeOp(2 * spp_2, tot));
}

void
DualBufferTile::AddSample(int64_t currentSampleIndex, const Point2f &pFilm, const Spectrum &L, Float sampleWeight) {
    (currentSampleIndex < spp_2 ? a : b)->AddSample(pFilm, L, sampleWeight);
    tot->AddSample(pFilm, L, sampleWeight);
    auto L2 = L * L;
    (currentSampleIndex < spp_2 ? a2 : b2)->AddSample(pFilm, L2, sampleWeight);
    tot2->AddSample(pFilm, L2, sampleWeight);
}

}
