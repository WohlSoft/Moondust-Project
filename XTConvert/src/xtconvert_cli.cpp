/*
 * XTConvert - a tool to package asset packs and episodes for TheXTech game engine.
 * Copyright (c) 2024 ds-sloth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <locale>
#include <iostream>
#include <stdio.h>

#include <Utf8Main/utf8main.h>
#include <tclap/CmdLine.h>
#include "libxtconvert.h"
#include "version.h"

int main(int argc, char** argv)
{
    XTConvert::Spec s;

    try
    {
        // Define the command line object.
        TCLAP::CmdLine  cmd(V_FILE_DESC "\n"
                            "Copyright (c) 2024 ds-sloth\n"
                            "This program is distributed under the GNU GPLv3+ license\n", ' ', V_FILE_VERSION V_FILE_RELEASE);

        TCLAP::UnlabeledValueArg<std::string> inputPath("input-path",
                "Path to input archive or directory",
                true,
                "Input file path (7z, zip, and directories supported)",
                "/path/to/input.7z or /path/to/input/directory");

        TCLAP::UnlabeledValueArg<std::string> outputPath("output-path",
                "Path to output archive",
                true,
                "Output path",
                "/path/to/output.xta/.xte/.romfs");

        TCLAP::ValueArg<std::string> baseAssetsPath("b", "base-assets",
            "Specify a non-default asset pack archive or directory for base GIF masks (useful for episodes targeting non-SMBX games)",
            false, "",
            "base assets path");

        TCLAP::SwitchArg switchAssetPack("a", "asset-pack", "Convert an asset pack (instead of an episode)", false);

        TCLAP::SwitchArg switchMain("m", "main", "Convert to mainline format (png)", false);
        TCLAP::SwitchArg switch3DS("3", "3ds", "Convert to 3DS-compatible format (t3x + spc2it)", false);
        TCLAP::SwitchArg switchWii("w", "wii", "Convert to Wii-compatible format (tpl)", false);
        TCLAP::SwitchArg switchDSi("d", "dsi", "Convert to DSi-compatible format (dsg + qoa + maxmod)", false);

        TCLAP::OneOf target_platform;
        target_platform.add(&switchMain).add(&switch3DS).add(&switchWii).add(&switchDSi);

        TCLAP::SwitchArg switchLegacyArchives("l", "legacy-archives", "For TheXTech v1.3.7: create a 7z archive (or romfs for 3DS) instead of ISO+LZ4", false);

        cmd.add(target_platform);
        cmd.add(&switchAssetPack);
        cmd.add(&baseAssetsPath);
        cmd.add(&switchLegacyArchives);
        cmd.add(&inputPath);
        cmd.add(&outputPath);

        cmd.parse(argc, argv);

        if((argc <= 1) || !inputPath.isSet() || !outputPath.isSet())
        {
            fprintf(stderr, "\n"
                    "ERROR: Missing input and output paths!\n"
                    "Type \"%s --help\" to display usage.\n\n", argv[0]);
            fflush(stdout);
            return 2;
        }

        if(switchMain.getValue())
            s.target_platform = XTConvert::TargetPlatform::Desktop;

        if(switch3DS.getValue())
            s.target_platform = XTConvert::TargetPlatform::T3X;

        if(switchWii.getValue())
            s.target_platform = XTConvert::TargetPlatform::TPL;

        if(switchDSi.getValue())
            s.target_platform = XTConvert::TargetPlatform::DSG;

        if(switchLegacyArchives.getValue())
            s.legacy_archives = true;

        if(switchAssetPack.getValue())
            s.package_type = XTConvert::PackageType::AssetPack;
        else
            s.package_type = XTConvert::PackageType::Episode;

        s.input_path = inputPath.getValue();
        s.destination = outputPath.getValue();
        s.base_assets_path = baseAssetsPath.getValue();
    }
    catch(TCLAP::ArgException &e)   // catch any exceptions
    {
        std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 2;
    }

    return !XTConvert::Convert(s);
}
