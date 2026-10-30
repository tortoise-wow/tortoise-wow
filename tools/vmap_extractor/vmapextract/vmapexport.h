/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef VMAPEXPORT_H
#define VMAPEXPORT_H

#include <string>
#include <set>
#include <vector>

typedef unsigned int uint32;
typedef std::set<std::string> StringSet;

enum ModelFlags
{
    MOD_M2 = 1,
    MOD_WORLDSPAWN = 1 << 1,
    MOD_HAS_BOUND = 1 << 2,
    MOD_NO_BREAK_LOS = 1 << 3,
};

class ModelLOSMgr
{
    public:
        class LosModificator
        {
            public:
            bool operator<(LosModificator const& b) const { return id < b.id; }
            bool enable;
            uint32 id;
            std::string filename;
        };
        static std::vector<LosModificator> modificators;
        static bool IsLOSEnabled(uint32 spawnId, std::string model);
        static bool Load();
};

extern const char* szWorkDirWmo;
extern const char* szRawVMAPMagic;                          // vmap magic string for extracted raw vmap data

bool FileExists(const char* file);
void strToLower(char* str);

bool ExtractSingleWmo(std::string& fname);

/* @param origPath = original path of the model, cleaned with fixnamen and fixname2
 * @param fixedName = will store the translated name (if changed)
 * @param failedPaths = Set to collect errors
 */
bool ExtractSingleModel(std::string& origPath, std::string& fixedName, StringSet& failedPaths);

void ExtractGameobjectModels();

/* Turtle's patches add models whose file name is already used by a different
 * stock model (world\custom\farm\farm.wmo next to the stock human_farm\farm.wmo).
 * Buildings/ is flat, so both were written to Buildings/Farm.wmo and every
 * placement of either model got whichever one was extracted first.
 *
 * FindModelNameCollisions() scans every archive's file list once and remembers
 * the file names that belong to more than one path. AddCollisionSuffix() then
 * gives those models a name unique to their path, e.g. Farm_h1a2b3c4d.wmo.
 * Every other model keeps the name it always had.
 */
void FindModelNameCollisions();
std::string NormalizeModelPath(std::string const& path);
std::string AddCollisionSuffix(std::string name, std::string const& fullPath);

#endif
