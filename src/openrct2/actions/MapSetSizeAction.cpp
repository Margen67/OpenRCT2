/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapSetSizeAction.h"

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Map.h"

MapSetSizeAction::MapSetSizeAction(uint16_t value)
    : _value(value)
{
}

void MapSetSizeAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("value", _value);
}

uint16_t MapSetSizeAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void MapSetSizeAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_value);
}

GameActions::Result::Ptr MapSetSizeAction::Query() const
{
    if (_value > MAXIMUM_MAP_SIZE_TECHNICAL)
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_INCREASE_MAP_SIZE_ANY_FURTHER);
    }
    if (_value < MINIMUM_MAP_SIZE_TECHNICAL) // 16
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_DECREASE_MAP_SIZE_ANY_FURTHER);
    }
    return MakeResult();
}

GameActions::Result::Ptr MapSetSizeAction::Execute() const
{
    while (_value > gMapSize)
    {
        gMapSize++;
        gMapSizeUnits = (gMapSize - 1) * 32;
        gMapSizeMinus2 = (gMapSize * 32) + MAXIMUM_MAP_SIZE_PRACTICAL;
        gMapSizeMaxXY = ((gMapSize - 1) * 32) - 1;
        map_extend_boundary_surface();
    }
    while (_value < gMapSize)
    {
        gMapSize--;
        gMapSizeUnits = (gMapSize - 1) * 32;
        gMapSizeMinus2 = (gMapSize * 32) + MAXIMUM_MAP_SIZE_PRACTICAL;
        gMapSizeMaxXY = ((gMapSize - 1) * 32) - 1;
        map_remove_out_of_range_elements();
    }

    gfx_invalidate_screen();

    return MakeResult();
}
