# Attiny85_cidla

Firmware pro ATtiny85, který periodicky napájí dvě optická čidla vodoměrů, sleduje sestupnou hranu jejich výstupu a při detekci vyšle přibližně 100 ms impulz na odpovídající výstup.

## Pinout

| Arduino pin | ATtiny85 port | DIP-8 pin | Funkce |
| --- | --- | ---: | --- |
| 0 | PB0 | 5 | Výstup impulzu – studená voda |
| 1 | PB1 | 6 | Napájení optosenzorů |
| 2 | PB2 | 7 | Výstup impulzu – teplá voda |
| 3 | PB3 | 2 | Vstup čidla – studená voda |
| 4 | PB4 | 3 | Vstup čidla – teplá voda |
| 5 | PB5 | 1 | RESET – nepoužívá se jako GPIO |

Napájení je na DIP-8 pinu 8 (VCC) a zem na pinu 4 (GND).

> Vstupy čidel jsou nastavené jako `INPUT`, stejně jako v původním firmware. Externí zapojení proto musí zajistit definovanou logickou úroveň. Interní pull-up rezistory nejsou zapnuté.

## Chování

1. ATtiny85 spí v režimu Power-down.
2. Watchdog jej probudí přibližně jednou za sekundu.
3. Zapne napájení optosenzorů a počká 2 ms na ustálení.
4. Přečte obě čidla.
5. Při přechodu HIGH -> LOW vyšle 100 ms impulz na příslušný výstup.
6. Optosenzory opět vypne a MCU se vrátí do Power-down.

Watchdog slouží jen jako zdroj probuzení, ne jako přesný časový etalon.

## Build

Projekt používá PlatformIO:

```bash
pio run
```

Konfigurace cílí na Generic ATtiny85 při 8 MHz:

```ini
[env:attiny85]
platform = atmelavr@5.3.0
board = attiny85
framework = arduino
board_build.f_cpu = 8000000L
```

Způsob nahrávání závisí na použitém programátoru. Generic ATtiny85 v PlatformIO standardně používá USBtiny; pokud používáš Arduino as ISP nebo jiný programátor, nastav odpovídající `upload_protocol`.

## Poznámka k měření

Firmware vzorkuje čidla přibližně jednou za sekundu, stejně jako původní návrh. Pokud může být aktivní stav čidla kratší než interval mezi dvěma vzorky, je nutné interval zkrátit nebo změnit hardwarovou/logickou koncepci měření.
