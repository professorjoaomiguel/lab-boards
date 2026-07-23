import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import gerar_indice as gi


class TestParseFrontMatter(unittest.TestCase):
    def test_parses_titulo_tipo_e_tags(self):
        with tempfile.TemporaryDirectory() as tmp:
            readme = Path(tmp) / "README.md"
            readme.write_text(
                '---\n'
                'titulo: "ESP32 DevKit V1"\n'
                'tipo: placa\n'
                'tags: [esp32, wifi, bluetooth]\n'
                '---\n'
                '# ESP32 DevKit V1\n',
                encoding="utf-8",
            )
            data = gi.parse_front_matter(readme)
            self.assertEqual(data["titulo"], "ESP32 DevKit V1")
            self.assertEqual(data["tipo"], "placa")
            self.assertEqual(data["tags"], ["esp32", "wifi", "bluetooth"])


class TestFindItems(unittest.TestCase):
    def test_encontra_itens_em_boards_e_shields(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            board_dir = root / "boards" / "esp32-devkit-v1"
            board_dir.mkdir(parents=True)
            (board_dir / "README.md").write_text(
                '---\n'
                'titulo: "ESP32 DevKit V1"\n'
                'tipo: placa\n'
                'tags: [esp32, wifi]\n'
                '---\n'
                '# ESP32\n',
                encoding="utf-8",
            )
            shield_dir = root / "shields" / "display-oled"
            shield_dir.mkdir(parents=True)
            (shield_dir / "README.md").write_text(
                '---\n'
                'titulo: "Display OLED"\n'
                'tipo: shield\n'
                'tags: [display, i2c]\n'
                '---\n'
                '# Display\n',
                encoding="utf-8",
            )

            items = gi.find_items(root=root)

            self.assertEqual(len(items), 2)
            titles = {item["titulo"] for item in items}
            self.assertEqual(titles, {"ESP32 DevKit V1", "Display OLED"})
            esp32_item = next(item for item in items if item["titulo"] == "ESP32 DevKit V1")
            self.assertEqual(esp32_item["link"], "boards/esp32-devkit-v1/README.md")


class TestGenerateIndex(unittest.TestCase):
    def test_agrupa_itens_por_tag(self):
        items = [
            {
                "titulo": "ESP32 DevKit V1",
                "tipo": "placa",
                "tags": ["esp32", "wifi"],
                "link": "boards/esp32-devkit-v1/README.md",
            },
            {
                "titulo": "Display OLED",
                "tipo": "shield",
                "tags": ["display", "i2c"],
                "link": "shields/display-oled/README.md",
            },
        ]
        content = gi.generate_index(items)
        self.assertIn("### esp32", content)
        self.assertIn(
            "- [ESP32 DevKit V1](boards/esp32-devkit-v1/README.md)", content
        )
        self.assertIn("### display", content)
        self.assertIn(
            "| ESP32 DevKit V1 | placa | esp32, wifi |"
            " [boards/esp32-devkit-v1/README.md](boards/esp32-devkit-v1/README.md) |",
            content,
        )


if __name__ == "__main__":
    unittest.main()
