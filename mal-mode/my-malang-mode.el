(require 'generic-x)

;;;###autoload
(let ()
  ;; define several category of keywords
  (setq my-mal-keywords '("break" "continue" "else" "for" "if" "return" "fn" "while"
                          "type" "extend"))
  (setq my-mal-implicits '("new" "self" "recurse" "it"))
  (setq my-mal-dragons '("breakpoint" "stack_trace" "gc_pause" "gc_resume" "gc_run"))
  ;; generate regex string for each category of keywords
  (setq my-mal-keywords-regexp (regexp-opt my-mal-keywords 'words))
  (setq my-mal-implicits-regexp (regexp-opt my-mal-implicits 'words))
  (setq my-mal-dragons-regexp (regexp-opt my-mal-dragons 'words))


  ;; create the list for font-lock.
  ;; each category of keyword is given a particular face
  (setq my-mal-font-lock
        `((,"\\(#.*\\)" 1 'font-lock-comment-face)
          (,"\\(\\?[^ \t]*\\)" 1 'font-lock-string-face)
          (,":[ \t]+\\([a-zA-Z_][a-zA-Z0-9_]*\\)" 1 'font-lock-type-face)
          (,"->[ \t]+\\([a-zA-Z_][a-zA-Z0-9_]*\\)" 1 'font-lock-type-face)
          (,"\\<import[ \t]" 0 'font-lock-preprocessor-face)
          (,"\\<import[ \t]\\([a-zA-Z0-9_$]+\\)" 1 'font-lock-string-face)
          (,my-mal-dragons-regexp . font-lock-warning-face)
          (,my-mal-implicits-regexp . font-lock-constant-face)
          (,my-mal-keywords-regexp . font-lock-keyword-face)
          ;; note: order above matters, because once colored, that part won't change.
          ;; in general, longer words first
          ))

  (define-derived-mode mal-mode prog-mode "malang mode"
    "Major mode for editing Malang"
    (setq font-lock-defaults '((my-mal-font-lock)))
    ))

(provide 'mal-mode)
