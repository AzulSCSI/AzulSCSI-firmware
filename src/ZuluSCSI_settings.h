/**
 * ZuluSCSI™ - Copyright (c) 2023 Rabbit Hole Computing™
 * Copyright (c) 2023 Eric Helgeson
 * 
 * This file is licensed under the GPL version 3 or any later version.  
 * 
 * https://www.gnu.org/licenses/gpl-3.0.html
 * ----
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
**/
#pragma once
#ifdef __cplusplus

#include <stdint.h>
#include <scsi2sd.h>

// Index 8 is the system defaults
// Index 0-7 represent device settings
#define SCSI_SETTINGS_SYS_IDX 8

typedef enum
{
    SYS_PRESET_NONE = 0,
    SYS_PRESET_MAC,
    SYS_PRESET_MACPLUS,
    SYS_PRESET_MPC3000,
    SYS_PRESET_MEGASTE,
    SYS_PRESET_X68000
} scsi_system_preset_t;

typedef enum
{
    DEV_PRESET_NONE = 0,
    DEV_PRESET_ST32430N
} scsi_device_preset_t;


// This struct should only have new settings added to the end
// as it maybe saved and restored directly from flash memory
typedef struct __attribute__((__packed__)) scsi_system_settings_t
{
    // Settings for host compatibility 
    uint8_t quirks;
    uint8_t selectionDelay;
    uint8_t maxSyncSpeed;
    uint8_t phyMode;
    uint16_t initPreDelay;
    uint16_t initPostDelay;

    bool enableUnitAttention;
    bool enableSCSI2;
    bool enableSelLatch;
    bool mapLunsToIDs;
    bool enableParity;
    bool useFATAllocSize;
    bool enableCDAudio;
    bool enableUSBMassStorage;
    uint16_t usbMassStorageWaitPeriod;

    bool invertStatusLed;

} scsi_system_settings_t;

// This struct should only have new setting added to the end
// as it maybe saved and restored directly from flash memory
typedef struct __attribute__((__packed__)) scsi_device_settings_t
{
    // Settings that can be set on all or specific device
    int prefetchBytes;
    uint16_t sectorsPerTrack;
    uint16_t headsPerCylinder;

    char prodId[16];
    char serial[16];
    char vendor[8];
    char revision[4];

    uint16_t vol;
    uint8_t deviceType;
    uint8_t deviceTypeModifier;
    uint8_t ejectButton;
    bool nameFromImage;
    bool rightAlignStrings;
    bool reinsertOnInquiry;
    bool reinsertAfterEject;
    bool disableMacSanityCheck;

    uint32_t sectorSDBegin;
    uint32_t sectorSDEnd;

    uint32_t vendorExtensions;

    uint32_t blockSize;
} scsi_device_settings_t;


class ZuluSCSISettings
{
public:
    // Initialize settings for all devices with a preset configuration,
    //  or return the default config if unknown system type.
    // Then overwrite any settings with those in the CONFIGFILE
    scsi_system_settings_t *initSystem(const char *presetName);

    // Copy any shared device setting done the initSystemSettings as default settings, 
    // or return the default config if unknown device type.
    // Then overwrite any settings with those in the CONFIGFILE
    scsi_device_settings_t *initDevice(uint8_t scsiId, S2S_CFG_TYPE type);
    // return the system settings struct to read values
    scsi_system_settings_t *getSystem();

    // return the device settings struct to read values
    scsi_device_settings_t *getDevice(uint8_t scsiId);

    // return the system preset enum
    scsi_system_preset_t getSystemPreset();

    // return the system preset name
    const char* getSystemPresetName();

    // return the device preset enum
    scsi_device_preset_t getDevicePreset(uint8_t scsiId);

    // return the device preset name
    const char* getDevicePresetName(uint8_t scsiId);

protected:
    // Set default drive vendor / product info after the image file
    // is loaded and the device type is known.
    void setDefaultDriveInfo(uint8_t scsiId, const char *presetName, S2S_CFG_TYPE type);

    // Settings for the specific device
    const char **deviceInitST32430N(uint8_t scsiId);

    // Informative name of the preset configuration, or NULL for defaults
    scsi_system_preset_t m_sysPreset;
    // The last preset is for the device specific under [SCSI] in the CONFIGFILE
    // The rest are for corresponding SCSI Ids e.g. [SCSI0] in the CONFIGFILE.
    scsi_device_preset_t m_devPreset[8];

    // These are setting for host compatibility
    scsi_system_settings_t m_sys;

    // The last dev will be copied over the other dev scsi Id for device defaults.
    // It is set during when the system settings are initialized
    scsi_device_settings_t m_dev[9];
} ;

extern ZuluSCSISettings g_scsi_settings;
#endif // __cplusplus
