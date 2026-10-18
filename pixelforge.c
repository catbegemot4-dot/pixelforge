/* PixelForge: запуск редактора в окне Chromium (режим --app, без вкладок и адресной строки).
 * Сборка:  gcc pixelforge.c -o pixelforge
 * Запуск:  ./pixelforge [путь/к/index.html]
 * Переменная CHROMIUM задаёт свой бинарник (chromium, google-chrome-stable и т.п.).
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef DATADIR /* Makefile подставляет $(PREFIX)/share/pixelforge */
#define DATADIR "/usr/local/share/pixelforge"
#endif

/* путь -> file:// URL с процентным кодированием (пробелы, кириллица) */
static void file_url(const char *path, char *out, size_t n) {
    static const char *ok = "/-_.~";
    size_t o = snprintf(out, n, "file://");
    for (const unsigned char *p = (const unsigned char *)path; *p && o + 4 < n; p++) {
        if ((*p >= '0' && *p <= '9') || ((*p | 32) >= 'a' && (*p | 32) <= 'z') || strchr(ok, *p))
            out[o++] = *p;
        else
            o += snprintf(out + o, n - o, "%%%02X", *p);
    }
    out[o] = 0;
}

int main(int argc, char **argv) {
    char real[PATH_MAX], url[PATH_MAX * 3 + 16], app[sizeof url + 8];
    char prof[PATH_MAX + 32], size[] = "--window-size=1400,860";

    /* порядок поиска: аргумент -> установленная копия -> ./index.html */
    const char *page = argc > 1 ? argv[1]
                     : access(DATADIR "/index.html", R_OK) == 0 ? DATADIR "/index.html"
                     : "index.html";
    if (!realpath(page, real)) {
        fprintf(stderr, "Файл index.html не найден\n");
        return 1;
    }
    file_url(real, url, sizeof url);
    snprintf(app, sizeof app, "--app=%s", url);

    const char *cache = getenv("XDG_CACHE_HOME"), *home = getenv("HOME");
    if (cache) snprintf(prof, sizeof prof, "--user-data-dir=%s/pixelforge", cache);
    else snprintf(prof, sizeof prof, "--user-data-dir=%s/.cache/pixelforge", home ? home : "/tmp");

    /* nix-shell вкладывает TMPDIR друг в друга, и путь к сокету Chromium
       перестаёт помещаться в лимит Unix-сокета (~108 байт). Берём короткий. */
    setenv("TMPDIR", "/tmp", 1);

    const char *env = getenv("CHROMIUM");
    const char *bins[] = { env, "chromium", "chromium-browser", "google-chrome-stable", NULL };
    for (int i = 0; i < 4; i++) {
        if (!bins[i]) continue;
        execlp(bins[i], bins[i], app, prof, size, "--no-first-run", "--no-default-browser-check", (char *)NULL);
    }
    fprintf(stderr, "Chromium не найден в PATH. В NixOS: nix-shell -p chromium\n");
    return 1;
}
