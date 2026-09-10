/*
 * vin_sensors.h
 *
 * History:
 *    2012/05/13 - [Rongrong Cao] Create
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __VIN_SENSORS_H__
#define __VIN_SENSORS_H__

/* SENSOR IDs */
#define GENERIC_SENSOR			(127)	//changed to 127 in new DSP firmware

/* RGB */
#define SENSOR_RGB_1PIX			(0)
#define SENSOR_RGB_2PIX			(1)
/* YUV */
#define SENSOR_YUV_1PIX			(2)
#define SENSOR_YUV_2PIX			(3)

/* YUV pixel order */
#define SENSOR_Y0_CB_Y1_CR		(0)
#define SENSOR_CR_Y0_CB_Y1		(1)
#define SENSOR_CB_Y0_CR_Y1		(2)
#define SENSOR_Y0_CR_Y1_CB		(3)

/* Interface type */
#define SENSOR_PARALLEL_LVCMOS		(0)
#define SENSOR_SERIAL_LVDS		(1)
#define SENSOR_PARALLEL_LVDS		(2)
#define SENSOR_MIPI			(3)
#define SENSOR_MIPI_VC			(4)
#define SENSOR_SLVS_EC			(5)

/* Lane number */
#define SENSOR_1_LANE			(1)
#define SENSOR_2_LANE			(2)
#define SENSOR_3_LANE			(3)
#define SENSOR_4_LANE			(4)
#define SENSOR_6_LANE			(6)
#define SENSOR_8_LANE			(8)
#define SENSOR_10_LANE			(10)
#define SENSOR_12_LANE			(12)

/* Sync code style */
#define SENSOR_SYNC_STYLE_SONY		(0)
#define SENSOR_SYNC_STYLE_HISPI		(1)
#define SENSOR_SYNC_STYLE_ITU656		(2)
#define SENSOR_SYNC_STYLE_PANASONIC	(3)
#define SENSOR_SYNC_STYLE_SONY_DOL	(4)
#define SENSOR_SYNC_STYLE_HISPI_PSP	(5)
#define SENSOR_SYNC_STYLE_INTERLACE	(6)
#define SENSOR_SYNC_STYLE_FPGA		(7)
#define SENSOR_SYNC_STYLE_SONY_DOL2	(8)

/* vsync/hsync polarity */
#define SENSOR_VS_HIGH			(0x1 << 1)
#define SENSOR_VS_LOW			(0x0 << 1)
#define SENSOR_HS_HIGH			(0x1 << 0)
#define SENSOR_HS_LOW			(0x0 << 0)

/* data dege */
#define SENSOR_DATA_RISING_EDGE	(0)
#define SENSOR_DATA_FALLING_EDGE	(1)

/* sync mode */
#define SENSOR_SYNC_MODE_MASTER	(0)
#define SENSOR_SYNC_MODE_SLAVE	(1)

/* paralle_sync_type */
#define SENSOR_PARALLEL_SYNC_656	(0)
#define SENSOR_PARALLEL_SYNC_601	(1)

/* mipi clock bit rate */
#define SENSOR_MIPI_BIT_RATE_M	(0)	/* default, ~400Mbps */
#define SENSOR_MIPI_BIT_RATE_H	(1)	/* ~800Mbps */
#define SENSOR_MIPI_BIT_RATE_L	(2)	/* ~200Mbps */
#define SENSOR_MIPI_BIT_RATE_UH	(3)	/* ~2200Mbps */

/* mipi data type */
#define SENSOR_MIPI_DATA_TYPE_RAW	(0)
#define SENSOR_MIPI_DATA_TYPE_ALL	(1)

/* mipi clock mode */
#define SENSOR_MIPI_CLK_CONTINUE	(0)
#define SENSOR_MIPI_CLK_NON_CONTINUE	(1)

/* mipi vc pattern */
#define SENSOR_MIPI_VC_ALL	(0)
#define SENSOR_MIPI_VC_0	(1)
#define SENSOR_MIPI_VC_1	(2)
#define SENSOR_MIPI_VC_2	(3)
#define SENSOR_MIPI_VC_3	(4)

/* mipi phy type */
#define SENSOR_MIPI_DPHY	(0)
#define SENSOR_MIPI_CPHY	(1)

/* shutter and gain setting timing */
#define SENSOR_SYNC_BIND_SOF	(0)
#define SENSOR_SYNC_BIND_EOF	(1)

/* parallel embedded sync location */
#define SENSOR_PARALLEL_SYNC_LOWER_PIX        (0)
#define SENSOR_PARALLEL_SYNC_UPPER_PIX        (1)
#define SENSOR_PARALLEL_SYNC_ACROSS_BOTH      (2)

/* parallel data rate */
#define SENSOR_PARALLEL_DATA_RATE_SDR         (0)
#define SENSOR_PARALLEL_DATA_RATE_DDR         (1)

/* parallel embedded sync location */
#define SENSOR_PARALLEL_NONE_A8_MODE          (0)
#define SENSOR_PARALLEL_A8_MODE               (1)

/* parallel hw specific setting */
#define SENSOR_PARALLEL_HW_BUB                (0)
#define SENSOR_PARALLEL_HW_CUSTOM             (1)

/* slvsec ecc mode */
#define SENSOR_SLVSEC_ECC_MODE_NONE	(0)
#define SENSOR_SLVSEC_ECC_MODE_1	(1)
#define SENSOR_SLVSEC_ECC_MODE_2	(2)

/* slvsec baud rate */
#define SENSOR_SLVSEC_BAUD_GRADE_2	(0)	/* default, 2304Mbps */
#define SENSOR_SLVSEC_BAUD_GRADE_3	(1)	/* 4608Mbps */
#define SENSOR_SLVSEC_BAUD_2376M	(2)	/* 2376Mbps */

#endif

