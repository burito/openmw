#include "loadland.hpp"

namespace ESM
{
void Land::load(ESMReader &esm)
{
    // Get the grid location
    esm.getSubNameIs("INTV");
    esm.getSubHeaderIs(8);
    esm.getT<int>(X);
    esm.getT<int>(Y);

    esm.getHNT(flags, "DATA");

    // Store the file position
    context = esm.getContext();

    hasData = false;
    int cnt = 0;

    // Skip these here. Load the actual data when the cell is loaded.
    if (esm.isNextSub("VNML"))
    {
        esm.skipHSubSize(12675);
        cnt++;
    }
    if (esm.isNextSub("VHGT"))
    {
        esm.skipHSubSize(4232);
        cnt++;
    }
    if (esm.isNextSub("WNAM"))
    {
        esm.skipHSubSize(81);
    }
    if (esm.isNextSub("VCLR"))
    {
        esm.skipHSubSize(12675);
    }
    if (esm.isNextSub("VTEX"))
    {
        esm.skipHSubSize(512);
        cnt++;
    }

    // We need all three of VNML, VHGT and VTEX in order to use the
    // landscape.
    hasData = (cnt == 3);

    dataLoaded = false;
    landData = NULL;
}

void Land::loadData(ESMReader &esm)
{
    if (dataLoaded)
    {
        return;
    }

    landData = new LandData;

    if (hasData)
    {
        esm.restoreContext(context);

        //esm.getHNExact(landData->normals, sizeof(VNML), "VNML");
        if (esm.isNextSub("VNML"))
        {
            esm.skipHSubSize(12675);
        }

        VHGT rawHeights;

        esm.getHNExact(&rawHeights, sizeof(VHGT), "VHGT");
        int currentHeightOffset = rawHeights.heightOffset;
        for (int y = 0; y < LAND_SIZE; y++)
        {
            currentHeightOffset += rawHeights.heightData[y * LAND_SIZE];
            landData->heights[y * LAND_SIZE] = currentHeightOffset * HEIGHT_SCALE;

            int tempOffset = currentHeightOffset;
            for (int x = 1; x < LAND_SIZE; x++)
            {
                tempOffset += rawHeights.heightData[y * LAND_SIZE + x];
                landData->heights[x + y * LAND_SIZE] = tempOffset * HEIGHT_SCALE;
            }
        }

        if (esm.isNextSub("WNAM"))
        {
            esm.skipHSubSize(81);
        }
        if (esm.isNextSub("VCLR"))
        {
            landData->usingColours = true;
            esm.getHExact(&landData->colours, 3*LAND_NUM_VERTS);
        }else{
            landData->usingColours = false;
        }
        //TODO fix magic numbers
        uint16_t vtex[512];
        esm.getHNExact(&vtex, 512, "VTEX");

        int readPos = 0; //bit ugly, but it works
        for ( int y1 = 0; y1 < 4; y1++ )
            for ( int x1 = 0; x1 < 4; x1++ )
                for ( int y2 = 0; y2 < 4; y2++)
                    for ( int x2 = 0; x2 < 4; x2++ )
                        landData->textures[(y1*4+y2)*16+(x1*4+x2)] = vtex[readPos++];
    }
    else
    {
        for (int i = 0; i < LAND_NUM_VERTS; i++)
        {
            landData->heights[i] = -256.0f * HEIGHT_SCALE;
        }
    }

    dataLoaded = true;
}

void Land::unloadData()
{
    if (dataLoaded)
    {
        delete landData;
        landData = NULL;
        dataLoaded = false;
    }
}

}
