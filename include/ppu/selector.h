#pragma once
#include "ppu/fetcher.h"

class PixelSelector {
    public:
        PixelSelector(PPU* parent_ppu) : 
            bg_fetcher(parent_ppu), sp_fetcher(parent_ppu) {};

    private:
        BGFetcher bg_fetcher;
        SPFetcher sp_fetcher;
};
