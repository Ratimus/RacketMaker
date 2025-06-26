// #pragma once

// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include <freertos/queue.h>
// #include <driver/spi_master.h>
// #include <driver/timer.h>
// #include <vector>
// #include <array>
// #include <cstdint>

// // Batch size can be changed dynamically
// constexpr size_t MAX_BATCH_SIZE = 128;
// constexpr size_t DAC_CHANNELS = 2;

// struct SampleBatch
// {
//   std::array<std::vector<uint16_t>, DAC_CHANNELS> samples;
//   size_t size = 0;
// };

// class SamplePlayer
// {
// public:
//   SamplePlayer(spi_host_device_t spi_host, gpio_num_t dac0_cs, gpio_num_t dac1_cs);
//   void begin();
//   void playSamples(const SampleBatch &batch);
//   void startPlaybackTimer(uint32_t sampleRateHz);

// private:
//   static void IRAM_ATTR onTimer();
//   static void dacTask(void *arg);
//   void sendBatchToDAC(const SampleBatch &batch);

//   spi_device_handle_t _dac0;
//   spi_device_handle_t _dac1;
//   QueueHandle_t _sampleQueue;

//   static SamplePlayer *_instance;
// };
