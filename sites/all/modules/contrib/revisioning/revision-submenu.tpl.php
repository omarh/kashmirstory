<?php
// $Id: revision-submenu.tpl.php,v 1.3 2009/04/30 03:26:48 rdeboer Exp $
/**
 * @file
 * revision-submenu.tpl.php, included by revision.tpl.php
 * This template handles the layout of the submenu that appears above the
 * body text of the displayed revision.
 *
 * Variables available:
 * - $submenu_links: an array of <a>-tags
 */
?>
<?php if ($submenu_links): ?>
  <div class="submenu revision">
    <?php print implode(' | ', $submenu_links); ?>
  </div>
  <hr/>
<?php endif; ?>
