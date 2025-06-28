// #pragma once

// #include <array>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include <freertos/queue.h>
// #include <driver/spi_master.h>

// constexpr size_t MAX_DAC_BATCH_SIZE = 128;

// struct DacChannelBatch
// {
//   size_t count;
//   std::array<uint16_t, MAX_DAC_BATCH_SIZE> samples;
// };

// class Mcp4822DacPlayer
// {
// public:
//   Mcp4822DacPlayer(spi_host_device_t host, gpio_num_t cs_pin, const char *task_name);
//   ~Mcp4822DacPlayer();

//   bool enqueueA(const DacChannelBatch &batch);
//   bool enqueueB(const DacChannelBatch &batch);

// private:
//   spi_device_handle_t spi_handle_;
//   QueueHandle_t queue_a_;
//   QueueHandle_t queue_b_;
//   TaskHandle_t task_handle_;

//   static void taskFunc(void *pvParams);
// };

// // Implementation
// #include <string.h>
// #include "esp_log.h"

// Mcp4822DacPlayer::Mcp4822DacPlayer(spi_host_device_t host, gpio_num_t cs_pin, const char *task_name)
// {
//   queue_a_ = xQueueCreate(4, sizeof(DacChannelBatch));
//   queue_b_ = xQueueCreate(4, sizeof(DacChannelBatch));

//   spi_device_interface_config_t devcfg = {
//       .clock_speed_hz = 10 * 1000 * 1000,
//       .mode = 0,
//       .spics_io_num = cs_pin,
//       .queue_size = 2,
//       .flags = SPI_DEVICE_HALFDUPLEX,
//   };

//   ESP_ERROR_CHECK(spi_bus_add_device(host, &devcfg, &spi_handle_));

//   xTaskCreatePinnedToCore(taskFunc, task_name, 4096, this, 1, &task_handle_, 1);
// }

// Mcp4822DacPlayer::~Mcp4822DacPlayer()
// {
//   vTaskDelete(task_handle_);
//   vQueueDelete(queue_a_);
//   vQueueDelete(queue_b_);
//   spi_bus_remove_device(spi_handle_);
// }

// bool Mcp4822DacPlayer::enqueueA(const DacChannelBatch &batch)
// {
//   return xQueueSend(queue_a_, &batch, 0) == pdTRUE;
// }

// bool Mcp4822DacPlayer::enqueueB(const DacChannelBatch &batch)
// {
//   return xQueueSend(queue_b_, &batch, 0) == pdTRUE;
// }

// void Mcp4822DacPlayer::taskFunc(void *pvParams)
// {
//   auto *self = static_cast<Mcp4822DacPlayer *>(pvParams);
//   DacChannelBatch batch;
//   spi_transaction_t t = {};

//   while (true)
//   {
//     if (xQueueReceive(self->queue_a_, &batch, 0) == pdTRUE)
//     {
//       t.length = batch.count * 16;
//       t.tx_buffer = batch.samples.data();
//       t.flags = 0;
//       spi_device_transmit(self->spi_handle_, &t);
//     }

//     if (xQueueReceive(self->queue_b_, &batch, 0) == pdTRUE)
//     {
//       t.length = batch.count * 16;
//       t.tx_buffer = batch.samples.data();
//       spi_device_transmit(self->spi_handle_, &t);
//     }

//     vTaskDelay(1);
//   }
// }

// // MAIN //////////////////
// #include <Arduino.h>
// #include "Mcp4822DacPlayer.h"

// // DAC player instances (GPIO 33 for DAC0, GPIO 32 for DAC1)
// Mcp4822DacPlayer dac0(SPI2_HOST, GPIO_NUM_33, "DAC0");
// Mcp4822DacPlayer dac1(SPI2_HOST, GPIO_NUM_32, "DAC1");

// void setup()
// {
//   // Initialize Arduino
//   Serial.begin(115200);
//   delay(100);
//   Serial.println("Starting triangle wave test");
// }

// void loop()
// {
//   DacChannelBatch batchA, batchB;
//   batchA.count = 128;
//   batchB.count = 128;

//   // Triangle wave up and down (0 to 4095 and back)
//   for (size_t i = 0; i < 64; ++i)
//   {
//     uint16_t val = i * 64; // 0 .. 4032
//     batchA.samples[i] = (0b0011 << 12) | (val & 0x0FFF);
//   }
//   for (size_t i = 64; i < 128; ++i)
//   {
//     uint16_t val = (127 - i) * 64; // 4032 .. 0
//     batchA.samples[i] = (0b0011 << 12) | (val & 0x0FFF);
//   }

//   // Offset B channel 90% (115 samples ahead)
//   for (size_t i = 0; i < 128; ++i)
//   {
//     size_t j = (i + 115) % 128;
//     batchB.samples[i] = batchA.samples[j];
//   }

//   dac0.enqueueA(batchA);
//   dac0.enqueueB(batchB);

//   delay(10); // Adjust for desired rate
// }

// ////////////////////////////////////////////////////
// #include <Arduino.h>
// #include "Mcp4822DacPlayer.h"

// // DAC player instances (GPIO 33 for DAC0, GPIO 32 for DAC1)
// Mcp4822DacPlayer dac0(SPI2_HOST, GPIO_NUM_33, "DAC0");
// Mcp4822DacPlayer dac1(SPI2_HOST, GPIO_NUM_32, "DAC1");

// constexpr size_t SAMPLE_LENGTH = 128;
// constexpr uint16_t TRIANGLE_AMPLITUDE = 4095;
// constexpr float SAMPLE_RATE_HZ = 44100.0f;
// constexpr int SAMPLE_PERIOD_US = static_cast<int>((1.0f / SAMPLE_RATE_HZ) * 1e6);

// // Pretend this is pre-recorded sample data
// std::array<uint16_t, SAMPLE_LENGTH> triangle_wave;

// void generateTriangleWave()
// {
//   for (size_t i = 0; i < SAMPLE_LENGTH / 2; ++i)
//   {
//     triangle_wave[i] = (0b0011 << 12) | ((i * TRIANGLE_AMPLITUDE / (SAMPLE_LENGTH / 2)) & 0x0FFF);
//   }
//   for (size_t i = SAMPLE_LENGTH / 2; i < SAMPLE_LENGTH; ++i)
//   {
//     triangle_wave[i] = (0b0011 << 12) | (((SAMPLE_LENGTH - i - 1) * TRIANGLE_AMPLITUDE / (SAMPLE_LENGTH / 2)) & 0x0FFF);
//   }
// }

// void setup()
// {
//   Serial.begin(115200);
//   delay(100);
//   Serial.println("Starting triangle sample playback");
//   generateTriangleWave();
// }

// void loop()
// {
//   DacChannelBatch batchA, batchB;
//   batchA.count = SAMPLE_LENGTH;
//   batchB.count = SAMPLE_LENGTH;

//   // Copy triangle waveform to channel A
//   for (size_t i = 0; i < SAMPLE_LENGTH; ++i)
//   {
//     batchA.samples[i] = triangle_wave[i];
//   }

//   // Copy phase-shifted waveform to channel B
//   size_t phase_offset = (SAMPLE_LENGTH * 90) / 100; // 90% phase shift
//   for (size_t i = 0; i < SAMPLE_LENGTH; ++i)
//   {
//     batchB.samples[i] = triangle_wave[(i + phase_offset) % SAMPLE_LENGTH];
//   }

//   dac0.enqueueA(batchA);
//   dac0.enqueueB(batchB);

//   delayMicroseconds(SAMPLE_PERIOD_US * SAMPLE_LENGTH); // 128-sample batch at 44.1kHz sample rate
// }

/*
SamplePlayer player(HSPI_HOST, GPIO_NUM_33, GPIO_NUM_32);

void setup()
{
    player.begin();
    player.startPlaybackTimer(44100);
}

void loop()
{
    SampleBatch batch;
    batch.size = 64;
    batch.samples[0].resize(batch.size);
    batch.samples[1].resize(batch.size);

    for (size_t i = 0; i < batch.size; ++i)
    {
        batch.samples[0][i] = (i * 64) & 0x0FFF;
        batch.samples[1][i] = ((128 - i) * 64) & 0x0FFF;
    }

    player.playSamples(batch);
    vTaskDelay(pdMS_TO_TICKS(10));
}
*/
