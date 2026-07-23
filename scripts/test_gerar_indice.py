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

    def test_ignora_readme_solto_na_raiz_da_secao(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "shields").mkdir(parents=True)
            (root / "shields" / "README.md").write_text(
                "# Shields\n\nAinda sem itens documentados.\n", encoding="utf-8"
            )
            board_dir = root / "boards" / "esp32-devkit-v1"
            board_dir.mkdir(parents=True)
            (board_dir / "README.md").write_text(
                '---\n'
                'titulo: "ESP32 DevKit V1"\n'
                'tipo: placa\n'
                'tags: [esp32]\n'
                '---\n'
                '# ESP32\n',
                encoding="utf-8",
            )

            items = gi.find_items(root=root)

            self.assertEqual(len(items), 1)
            self.assertEqual(items[0]["titulo"], "ESP32 DevKit V1")


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


class TestFrontMatterComentarioInline(unittest.TestCase):
    def test_remove_comentario_inline_do_valor_de_tipo(self):
        with tempfile.TemporaryDirectory() as tmp:
            readme = Path(tmp) / "README.md"
            readme.write_text(
                '---\n'
                'titulo: "Item com comentario"\n'
                'tipo: placa            # placa | shield\n'
                'tags: [teste]\n'
                '---\n'
                '# Item\n',
                encoding="utf-8",
            )
            data = gi.parse_front_matter(readme)
            self.assertEqual(data["tipo"], "placa")


class TestParseFrontMatterErroDeFechamento(unittest.TestCase):
    def test_levanta_erro_sem_delimitador_de_fechamento(self):
        with tempfile.TemporaryDirectory() as tmp:
            readme = Path(tmp) / "README.md"
            readme.write_text(
                '---\ntitulo: "Sem fechamento"\ntipo: placa\n# falta o --- final\n',
                encoding="utf-8",
            )
            with self.assertRaises(ValueError):
                gi.parse_front_matter(readme)


class TestItemSemTags(unittest.TestCase):
    def test_tags_ausentes_ou_vazias_viram_lista_vazia(self):
        with tempfile.TemporaryDirectory() as tmp:
            readme = Path(tmp) / "README.md"
            readme.write_text(
                '---\ntitulo: "Sem tags"\ntipo: placa\ntags: []\n---\n# Sem tags\n',
                encoding="utf-8",
            )
            data = gi.parse_front_matter(readme)
            self.assertEqual(data["tags"], [])

    def test_generate_index_nao_quebra_com_item_sem_tags(self):
        items = [
            {
                "titulo": "Sem tags",
                "tipo": "placa",
                "tags": [],
                "link": "boards/sem-tags/README.md",
            },
        ]
        content = gi.generate_index(items)
        self.assertIn(
            "| Sem tags | placa |  | [boards/sem-tags/README.md](boards/sem-tags/README.md) |",
            content,
        )


if __name__ == "__main__":
    unittest.main()
