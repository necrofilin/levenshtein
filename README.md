#Installation
To build extension run
```sh
 vendor/necrofilin/levenshtein/build.sh
```

Or add
```json
"post-install-cmd": [
    "bash vendor/necrofilin/levenshtein/build.sh"
],
"post-update-cmd": [
    "bash vendor/necrofilin/levenshtein/build.sh"
]
```
  to `script` section in your `composer.json`

#Usage
See [Levenshtein class dummy](src/Nfilin/Libs/Levenshtein.php) for details.
