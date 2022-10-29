/* ====================================
   Desenvolvido por Jefferson Dantas
   ==================================== */

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Pinos do LCD
#define TFT_DC 9
#define TFT_CS 10
#define TFT_MOSI 8
#define TFT_CLK 7
#define TFT_RST 6
#define TFT_MISO 5

const int16_t speed = 300; // (300) Velocidade de atualização dos dados
const float chartMax = 110; // (110) Maior valor na escala do gráfico
const float chartMin = -110; // (-110) Menor valor na escala do gráfico
const int8_t chartPadding = 10; // (10) Espaçamento dentro da área do gráfico
const int8_t chartScale = 10; // (10) Escala vertical do gráfico
const int16_t chartDistance = 20; // (20) Distância horizontal entre os pontos
const int16_t chartXOffset = 35; // (35) Posição X da área do gráfico
const int16_t chartYOffset = 0; // (0) Posição Y da área do gráfico
const int16_t chartWidth = 320 - chartXOffset; // (285) Largura da área do gráfico
const int16_t chartHeight = 240; // (240) Altura da área do gráfico
const int16_t chartBackground = 0x2124; // (0x2124) Cor de fundo do gráfico
const int16_t chartBorderColor = ILI9341_WHITE; // (ILI9341_WHITE) Cor da borda do gráfico
const int16_t chartScaleColor = ILI9341_WHITE; // (ILI9341_WHITE) Cor da borda da escala
const int16_t chartScaleTextColor = ILI9341_WHITE; // (ILI9341_WHITE) Cor do texto da escala
const int16_t chartHGridColor = 0x9492; // (0x9492) Cor da grade horizontal do gráfico
const int16_t chartVGridColor = 0x52AA; // (0x52AA) Cor da grade vertical do gráfico
const int16_t chartLineColor = ILI9341_YELLOW; // (ILI9341_YELLOW) Cor da linha do gráfico
const int16_t chartMarkerColor = ILI9341_RED; // (ILI9341_RED) Cor do marcador do gráfico
const bool showValueBox = true; // (true) Exibe uma caixa com o valor atual

// Não alterar os valores abaixo
const float delta = chartMax - chartMin; // Variação entre o maior valor e o menor (não alterar)
const int16_t chartPXOffset = chartXOffset + chartPadding; // Posição X da área do gráfico + chartPadding (não alterar)
const int16_t chartPYOffset = chartYOffset + chartPadding; // Posição Y da área do gráfico + chartPadding (não alterar)
const int16_t chartPWidth = chartWidth - chartPadding * 2; // Largura da área do gráfico + chartPadding (não alterar)
const int16_t chartPHeight = chartHeight - chartPadding * 2; // Altura da área do gráfico + chartPadding (não alterar)
const int8_t dataQuantity = chartPWidth / chartDistance + 1; // Quantidade de dados que cabem no gráfico (não alterar)
int16_t data[dataQuantity]; // Dados do gráfico

// LCD
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  Serial.begin(9600);
  TFT.begin();
  TFT.setRotation(1);
}

void loop() {
  drawChartBox(); // Desenha a caixa do gráfico
  drawChartScale(); // Desenha a escala vertical do gráfico

  while (1) {
    drawHorizontalGrid(); // Desenha a Grade Horizontal
    drawVerticalGrid(); // Desenha a Grade Vertical
    drawChartData(); // Desenha os dados do gráfico
    eraseChartData(); // Apaga os dados do gráfico
  }
}

// Desenha a caixa do gráfico
void drawChartBox() {
  TFT.drawRect(chartXOffset - 1, chartYOffset - 1, chartWidth + 2, chartHeight + 2, chartBorderColor); // Borda do gráfico
  TFT.fillRect(chartXOffset, chartYOffset, chartWidth, chartHeight, chartBackground); // Fundo do gráfico
}

// Desenha a escala vertical do gráfico
void drawChartScale() {
  TFT.drawLine(chartXOffset - 5, chartPYOffset, chartXOffset - 5, chartYOffset + chartHeight - chartPadding, chartScaleColor); // Linha vertical da escala do gráfico
  for (int8_t i = 0; i < (chartPHeight / chartScale + 1); i++) {
    // Linhas horizontais da escala do gráfico
    TFT.drawLine(
      chartXOffset - 5,
      chartYOffset + chartHeight - chartPadding - chartScale * i,
      chartXOffset - 10,
      chartYOffset + chartHeight - chartPadding - chartScale * i,
      chartScaleColor
    );

    // Textos da escala do gráfico
    TFT.setCursor(chartXOffset - 47, chartYOffset + chartHeight - chartPadding - chartScale * i - 4);
    TFT.setTextColor(chartScaleTextColor);
    String text = String(round(delta / chartPHeight * (i * chartScale + chartMin)));
    for (int8_t j = text.length(); j < 6; j++) text = " " + text;
    TFT.setTextSize(1);
    TFT.print(text);
  }
}

// Desenha a Grade Horizontal
void drawHorizontalGrid() {
  for (int8_t i = (chartPadding == 0 ? 1 : 0); i < (chartHeight / chartScale); i++) {
    TFT.drawLine(
      chartXOffset,
      chartYOffset + chartHeight - chartPadding - chartScale * i,
      chartXOffset + chartWidth - 1,
      chartYOffset + chartHeight - chartPadding - chartScale * i,
      chartHGridColor
    );
  }
}

// Desenha a Grade Vertical
void drawVerticalGrid() {
  for (int8_t i = 0; i < dataQuantity; i++) {
    TFT.drawLine(
      chartPXOffset + chartDistance * i,
      chartYOffset,
      chartPXOffset + chartDistance * i,
      chartYOffset + chartHeight - 1,
      chartVGridColor
    );
  }
}

// Desenha os dados do gráfico
void drawChartData() {
  int16_t prevValue;
  for (int8_t column = 0;; column++) {
    const float value = random(chartMin, chartMax + 1);
    drawValueBox(value);
    data[column] = value;
    if (column == 0) {
      prevValue = value;
      continue;
    }
    const int16_t x1 = chartPXOffset + (column - 1 < 0 ? 0 : column - 1) * chartDistance;
    const int16_t y1 = chartPYOffset + chartPHeight - chartPHeight / delta * prevValue + chartMin;
    const int16_t x2 = chartPXOffset + column * chartDistance;
    const int16_t y2 = chartPYOffset + chartPHeight - chartPHeight / delta * value + chartMin;
    if (column * chartDistance > chartPWidth) break;
    TFT.drawLine(x1, y1, x2, y2, chartLineColor);
    TFT.fillCircle(x1, y1, 2, chartMarkerColor);
    TFT.fillCircle(x2, y2, 2, chartMarkerColor);
    prevValue = value;
    delay(speed);
  }
}

// Apaga os dados do gráfico
void eraseChartData() {
  for (int8_t i = 1; i < dataQuantity; i++) {
    const int16_t prevValue = data[i - 1];
    const float value = data[i];
    const int16_t x1 = chartPXOffset + (i - 1 < 0 ? 0 : i - 1) * chartDistance;
    const int16_t y1 = chartPYOffset + chartPHeight - chartPHeight / delta * prevValue + chartMin;
    const int16_t x2 = chartPXOffset + i * chartDistance;
    const int16_t y2 = chartPYOffset + chartPHeight - chartPHeight / delta * value + chartMin;
    TFT.drawLine(x1, y1, x2, y2, chartBackground);
    TFT.fillCircle(x1, y1, 2, chartBackground);
    TFT.fillCircle(x2, y2, 2, chartBackground);
  }
}

// Desenha a caixa com o valor atual
void drawValueBox(float value) {
  if (!showValueBox) return;
  const int16_t width = 50;
  const int16_t height = 20;
  const int16_t x = 320 - width;
  const int16_t y = 240 - height;
  TFT.drawRect(x, y, width, height, ILI9341_WHITE);
  TFT.fillRect(x + 1, y + 1, width - 2, height - 2, ILI9341_BLUE);
  TFT.setCursor(x + 4, y + height / 2 - 3);
  TFT.print(value);
}
