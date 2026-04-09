# PSOC™ Edge Training - Trusted Firmware-M (TF-M)

This training is part of the PSOC™ Edge Security training series and focuses on Trusted Firmware-M (TF-M). TF-M provides a Secure Processing Environment (SPE) and serves as the reference implementation of the Platform Security Architecture (PSA). This training outlines the TF-M architecture in PSOC™ Edge, including boot flow, isolation levels, profiles, and services. It also covers default TF-M configurations,application structure with TF-M, and the use of the Edge Protect Configurator for customization.

## Device family
- [PSOC™ Edge](https://www.infineon.com/products/microcontroller/32-bit-psoc-arm-cortex/32-bit-psoc-edge-arm)

## How to use this training?
1. Download the training [content](#content).
2. Watch the video or review the presentation at your own pace.
3. Follow the step-by-step instructions in the training manual during the hands-on sections.

## Training level
- E3: Advanced

## Pre-requisites 
### Recommended trainings
- This training doesn't cover basic concepts of ModusToolbox™ and PSOC™ Edge. 
  - For an introduction to PSOC™ MCUs, including getting started guides to ModusToolbox™, go to the [PSOC™ Developer Journey](https://www.infineon.com/PSOCdeveloper).
  - For PSOC™ Edge trainings, from beginner tutorials to advanced trainings, please visit the [PSOC™ Edge E84 Training Collection](https://infineon-academy.csod.com/ui/lms-learner-playlist/PlaylistDetails?playlistId=8f04565f-88f4-4ca7-83b3-22e501656fbd).
- For an introduction to PSOC™ Edge Security, visit https://github.com/Infineon/mtb-training-psoc-edge-security-intro

### Tools (see [training manual](#content) for versions and installation instructions)
- [ModusToolbox™ with Eclipse IDE](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxsetup)
- [Edge Protect Security Suite](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxsetup)
- [ModusToolbox™ Programming tools](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxsetup)
- Terminal emulator

## Hardware
- [KIT_PSE84_EVAL](https://www.infineon.com/evaluation-board/KIT-PSE84-EVAL)

## Duration
- 1–2 hours, including video and hands-on labs

## Agenda
1. TF-M overview
2. TF-M architecture in PSOC™ Edge
3. Boot flow with TF-M
4. Isolation levels
5. TF-M profiles
6. TF-M services
7. Default TF-M configurations
8. Application structure with TF-M
9. Lab #1: TF-M Crypto code example
10. Lab #2: Adding new application RoT partition

## Expected outcome
- Become familiar with TF-M and its implementation on PSOC™ Edge
- Understand TF-M services and how to use them

## Content
- Training video at Infineon Academy (coming soon)
- [Presentation](./Presentation/PSE84_Trusted_Firmware_M.pdf)
- [Training manual](./Manual/PSE84_Trusted_Firmware_M_TrainingManual.pdf)

## References and resources
- [PSOC™ Edge MCUs](https://www.infineon.com/products/microcontroller/32-bit-psoc-arm-cortex/32-bit-psoc-edge-arm)
- [Infineon Edge Protect](https://www.infineon.com/promo/edge-protect)
- [Introduction to PSOC™ MCUs and ModusToolbox™](https://www.infineon.com/PSOCdeveloper)
- [PSOC™ Edge E84 training collection](https://infineon-academy.csod.com/samldefault.aspx?ouid=1&returnURL=%252fDeepLink%252fProcessRedirect.aspx%253fmodule%253dphnxdriver%2526routename%253dAdmin%252fPlayerPageRedirectHandler%2526Route%253d%25252flms-learner-playlist%25252fPlaylistDetails%2526Parameters%253dplaylistId%25253d8f04565f-88f4-4ca7-83b3-22e501656fbd)

## History 
| Date       | Version | Description             |
|------------|---------|-------------------------|
| 02/16/2026 | **      | First public release    |