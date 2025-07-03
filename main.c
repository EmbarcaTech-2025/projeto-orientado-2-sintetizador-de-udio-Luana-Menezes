#include <stdio.h> // Biblioteca padrão
#include <math.h>  // Biblioteca de matemática (função "round" foi utilizada)
#include <string.h>

#include "pico/stdlib.h"   // Biblioteca padrão pico
#include "hardware/gpio.h" // Biblioteca de GPIOs
#include "hardware/adc.h"  // Biblioteca do ADC
#include "hardware/pwm.h"  // Biblioteca do PWM

#include "include/ssd1306.h"

#define MIC_PIN 28
#define ADC_CHANNEL (MIC_PIN - 26)

#define SAMPLE_RATE_HZ 8000
#define SAMPLE_DELAY_US (1000000 / SAMPLE_RATE_HZ)

#define RECORDING_DURATION_S 10

#define BUFFER_SIZE (SAMPLE_RATE_HZ * RECORDING_DURATION_S)
uint8_t audio_buffer[BUFFER_SIZE];

#define BUZZER_PIN 21

#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

#define LED_GREEN_PIN 11
#define LED_RED_PIN 13

volatile bool button_a_pressed = false;
volatile bool button_b_pressed = false;

volatile uint last_button_a_time = 0;
volatile uint last_button_b_time = 0;

#define DEBOUNCE_TIME_MS 300

#define DISPLAY_I2C_PORT i2c1
#define DISPLAY_I2C_SDA 14
#define DISPLAY_I2C_SCL 15

#define ADC_MAX_VALUE 255

struct render_area frame_area = {
  start_column : 0,
  end_column : ssd1306_width - 1,
  start_page : 0,
  end_page : ssd1306_n_pages - 1
};

uint8_t display[ssd1306_buffer_length];

void init_adc()
{
  adc_init();
  adc_gpio_init(MIC_PIN);
  adc_select_input(ADC_CHANNEL);
}

void record_audio()
{
  printf("Recording audio for %d seconds... Talk now!\n", RECORDING_DURATION_S);
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    uint16_t adc_12bits = adc_read();
    audio_buffer[i] = adc_12bits >> 4;
    sleep_us(SAMPLE_DELAY_US);

    if (i % SAMPLE_RATE_HZ == 0)
    {
      printf("Recording second %d of %d\n", (i / SAMPLE_RATE_HZ) + 1, RECORDING_DURATION_S);
    }
  }
  printf("Recording complete! %d samples\n", BUFFER_SIZE);
}

void init_pwm()
{
  gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 1.0f);
  pwm_config_set_wrap(&config, 4095);
  pwm_init(slice_num, &config, false);
}

void play_audio()
{
  printf("Playing sound on buzzer...\n");

  uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
  pwm_set_enabled(slice_num, true);

  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    uint16_t pwm_12bits = audio_buffer[i] << 4;
    ;
    pwm_set_gpio_level(BUZZER_PIN, pwm_12bits);

    sleep_us(SAMPLE_DELAY_US);

    if (i % SAMPLE_RATE_HZ == 0)
    {
      printf("Playing second %d of %d\n", (i / SAMPLE_RATE_HZ) + 1, RECORDING_DURATION_S);
    }
  }

  pwm_set_gpio_level(BUZZER_PIN, 0);
  sleep_us(100);
  pwm_set_enabled(slice_num, false);
  printf("Sound playback complete!\n");
}

void gpio_callback(uint gpio, uint32_t events)
{
  uint now = to_ms_since_boot(get_absolute_time());
  if (gpio == BUTTON_A_PIN && ((now - last_button_a_time) > DEBOUNCE_TIME_MS))
  {
    button_a_pressed = true;
    last_button_a_time = now;
  }

  if (gpio == BUTTON_B_PIN && ((now - last_button_b_time) > DEBOUNCE_TIME_MS))
  {
    button_b_pressed = true;
    last_button_b_time = now;
  }
}

void init_buttons()
{
  gpio_init(BUTTON_A_PIN);
  gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_A_PIN);

  gpio_init(BUTTON_B_PIN);
  gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_B_PIN);

  gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
  gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);
}

void init_leds()
{
  gpio_init(LED_RED_PIN);
  gpio_set_dir(LED_RED_PIN, GPIO_OUT);
  gpio_put(LED_RED_PIN, false);

  gpio_init(LED_GREEN_PIN);
  gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
  gpio_put(LED_GREEN_PIN, false);
}

void set_led_red(bool state)
{
  gpio_put(LED_RED_PIN, state);
}

void set_led_green(bool state)
{
  gpio_put(LED_GREEN_PIN, state);
}

void display_clear()
{
  memset(display, 0, ssd1306_buffer_length);
  render_on_display(display, &frame_area);
}

void init_display()
{
  i2c_init(DISPLAY_I2C_PORT, ssd1306_i2c_clock * 1000);
  gpio_set_function(DISPLAY_I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(DISPLAY_I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(DISPLAY_I2C_SDA);
  gpio_pull_up(DISPLAY_I2C_SCL);

  ssd1306_init();
  calculate_render_area_buffer_length(&frame_area);
  display_clear();
}

void display_audio_waveform()
{
  display_clear();

  int samples_per_pixel = BUFFER_SIZE / ssd1306_width;

  int center_y = ssd1306_height / 2;

  for (int x = 0; x < ssd1306_width; x++)
  {
    int sample_i = x * samples_per_pixel;
    if (sample_i >= BUFFER_SIZE)
      break;
    uint audio_val = audio_buffer[sample_i];

    int amplitude = abs((int)audio_val - (ADC_MAX_VALUE / 2));

    int bar_height = (amplitude * center_y) / (ADC_MAX_VALUE / 2);
    if (bar_height > center_y)
      bar_height = center_y;

    int y_top = center_y - bar_height;
    int y_bottom = center_y + bar_height;

    ssd1306_draw_line(display, x, y_top, x, y_bottom, true);
  }
  render_on_display(display, &frame_area);
}

void audio_high_pass_filter()
{
  uint average = 0;
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    average += audio_buffer[i];
  }
  average = average / BUFFER_SIZE;

  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    int new_value = (int)audio_buffer[i] - (int)average + 127;

    if (new_value < 0) new_value = 0;
    if (new_value > 255) new_value = 255;

    audio_buffer[i] = new_value;
  }
}

int main()
{
  stdio_init_all();
  sleep_ms(2000);

  init_adc();
  init_pwm();
  init_buttons();
  init_leds();
  init_display();

  printf("Program started!\n");

  while (1)
  {
    if (button_a_pressed)
    {
      button_a_pressed = false;
      display_clear();
      set_led_red(true);
      record_audio();
      audio_high_pass_filter();
      set_led_red(false);
      display_audio_waveform();
    }
    if (button_b_pressed)
    {
      button_b_pressed = false;
      set_led_green(true);
      play_audio();
      set_led_green(false);
    }
    sleep_ms(200);
  }
  return 0;
}