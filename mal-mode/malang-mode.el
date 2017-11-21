;;; malang-mode-el -- Major mode for editing malang source code

;; Author: Max Mickey
;; Keywords: malang major-mode

;; Copyright (C) 2017 Max Mickey

;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License as
;; published by the Free Software Foundation; either version 2 of
;; the License, or (at your option) any later version.

;; This program is distributed in the hope that it will be
;; useful, but WITHOUT ANY WARRANTY; without even the implied
;; warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
;; PURPOSE.  See the GNU General Public License for more details.

;; You should have received a copy of the GNU General Public
;; License along with this program; if not, write to the Free
;; Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
;; MA 02111-1307 USA

;;; Commentary:
;; 
;; This mode is meant for editing malang source files.
;; It is heavily based on https://www.emacswiki.org/emacs/wpdl-mode.el

(require 'generic-x)
(require 'font-lock)

;;;###autoload
(let ()
  (defvar mal-mode-hook nil)
  (defvar mal-mode-map
    (let ((mal-mode-map (make-keymap)))
      (define-key mal-mode-map "\C-j" 'newline-and-indent)
      mal-mode-map)
    "Keymap for mal-mode")

  (add-to-list 'auto-mode-alist '("\\.ma'" . mal-mode))

  ;; define several category of keywords
  (setq my-mal-keywords '("break" "continue" "else" "for" "if" "return" "fn" "while"
                          "type" "alias" "unalias" "extend"))
  (setq my-mal-implicits '("new" "self" "recurse" "it" "true" "false"))
  (setq my-mal-dragons '("breakpoint" "stack_trace" "gc_pause" "gc_resume" "gc_run"))
  ;; generate regex string for each category of keywords
  (setq my-mal-keywords-regexp (regexp-opt my-mal-keywords 'words))
  (setq my-mal-implicits-regexp (regexp-opt my-mal-implicits 'words))
  (setq my-mal-dragons-regexp (regexp-opt my-mal-dragons 'words))


  ;; create the list for font-lock.
  ;; each category of keyword is given a particular face
  (setq mal-font-lock-keywords
        `(;(,"\\(#.*\\)" 1 'font-lock-comment-face)
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

  (defun mal-indent-line ()
    "Indent current line as malang code."
    (interactive)
    (beginning-of-line)
    (if (bobp)
        (indent-line-to 0)		   ; First line is always non-indented
      (let ((not-indented t) cur-indent)
        (if (looking-at ".*?}") ; If the line we are looking at is the end of a block, then decrease the indentation
            (progn
              (save-excursion
                (forward-line -1)
                (setq cur-indent (- (current-indentation) tab-width)))
              (if (< cur-indent 0) ; We can't indent past the left margin
                  (setq cur-indent 0)))
          (save-excursion
            (while not-indented ; Iterate backwards until we find an indentation hint
              (forward-line -1)
              (if (looking-at ".*?}") ; This hint indicates that we need to indent at the level of the } token
                  (progn
                    (setq cur-indent (current-indentation))
                    (setq not-indented nil))
                (if (looking-at ".*?{") ; This hint indicates that we need to indent an extra level
                    (progn
                      (setq cur-indent (+ (current-indentation) tab-width)) ; Do the actual indenting
                      (setq not-indented nil))
                  (if (bobp)
                      (setq not-indented nil)))))))
        (if cur-indent
            (indent-line-to cur-indent)
            ; If we didn't see an indentation hint, then allow no indentation
          (indent-line-to 0))))) 

  (defvar mal-mode-syntax-table
    (let ((mal-mode-syntax-table (make-syntax-table)))

      (modify-syntax-entry ?_ "w" mal-mode-syntax-table)
      (modify-syntax-entry ?# "<" mal-mode-syntax-table)
      (modify-syntax-entry ?\n ">" mal-mode-syntax-table)
      mal-mode-syntax-table)
    "Syntax table for mal-mode")


  (defun mal-mode ()
    (interactive)
    (kill-all-local-variables)
    (use-local-map mal-mode-map)
    (set-syntax-table mal-mode-syntax-table)
    (set (make-local-variable 'font-lock-defaults) '(mal-font-lock-keywords))
    (set (make-local-variable 'indent-line-function) 'mal-indent-line)
    (setq major-mode 'mal-mode)
    (setq mode-name "malang mode")
    (run-hooks 'mal-mode-hook)))

(provide 'mal-mode)
