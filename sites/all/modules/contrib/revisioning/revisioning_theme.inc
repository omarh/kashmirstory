<?php
// $Id: revisioning_theme.inc,v 1.9 2009/05/12 07:08:07 rdeboer Exp $

/**
 * @file
 * Include file for revisioning.module; deals with all theming aspects.
 */

/**
 * Implementation of hook_theme().
 * Register the theme_hooks() available in this module, with their arguments
 * and default values.
 */
function revisioning_theme() {
  return array(
    'revisioning_revisions_summary' => array(
      'arguments' => array('form' => array()),
    ),
    'revision_submenu' => array(
      'arguments' => array('submenu_links' => NULL),
      'template' => 'revision-submenu', // revision-submenu.tpl.php
    ),
    'revision' => array(
      'arguments' => array('submenu_links' => NULL, 'content' => NULL),
      'template' => 'revision', // revision.tpl.php
    ),
    'revisions_summary' => array(
      'arguments' => array('submenu_links' => NULL, 'content' => NULL),
      'template' => 'revisions-summary', // revisions-summary.tpl.php
    )
  );
}

/**
 * Theme the revisions summary of the supplied node.
 *
 * @param $node
 *   Node whose revisions to display
 * @return
 *   Themed table HTML
 *
 * @ingroup themeable
 */
function _theme_revisions_summary(&$node) {
  drupal_set_title(t('Revisions for %title', array('%title' => $node->title)));
  return drupal_get_form('revisioning_revisions_summary', $node->nid);
}

/**
 * Return revisions summary table data. If the Diff modules is enabled, the
 * object returned includes a column of checkboxes allowing the user to select
 * two revisions for side-by-side comparison.
 * 
 * @param $form_state
 * @param $nid, the id of the node
 * @return form containing all data to be themed
 */
function revisioning_revisions_summary($form_state, $nid) {
  $form = array();
  
  // #type=>'value' form field values will not appear in the HTML. Used to pass
  // the node id to revisioning_revisions_summary_submit() and 
  // theme_revisioning_revisions_summary().
  $form['nid'] = array('#type' => 'value', '#value' => $nid);

  $show_taxonomy_terms = module_exists('taxonomy');
  $revisions = _get_all_revisions_for_node($nid, $show_taxonomy_terms);

  drupal_set_message(format_plural(count($revisions),
    'This content has only one revision', 
    'This content has @count revisions.'));

  $revision_ids = array();
  foreach ($revisions as $revision) {
    $vid = $revision->vid;
    $revision_ids[$vid] = ''; // no text next to check box 
    $base_url = "node/$nid/revisions/$vid";

    // First column: saved date + author    
    $first_cell = t('Saved !date by !username', 
      array('!date' => l(format_date($revision->timestamp, 'small'), "$base_url/view"),
            '!username' => theme('username', $revision)))
      . (empty($revision->log) ? '' : '<p class="revision-log">'. filter_xss($revision->log) .'</p>');
    $form['info'][$vid] = array('#value' => $first_cell);
    
    // Third column: term
    if ($show_taxonomy_terms) {
      $form['term'][$vid] = array('#value' => $revision->term);
    }
  }
  if (count($revisions) >= 2 && module_exists('diff')) {
    // Second column: check-boxes to select two revisions to compare
    $id1 = key($revision_ids);
    next($revision_ids);
    $id2 = key($revision_ids);
    $form['tickbox'] = array(
      '#type' => 'checkboxes', 
      '#options' => $revision_ids,
      '#default_value' => array($id1, $id2),
      '#required' => TRUE,
    );
    // Submit button
    $form['submit'] = array('#value' => t('Compare'), '#type' => 'submit');
  }
  return $form;
}

/**
 * Validation for input form to select two revisions.
 * 
 * @param $form
 * @param $form_state
 * @return void
 */
function revisioning_revisions_summary_validate($form, &$form_state) {
  // Strip out all unchecked boxes
  $form_state['values']['tickbox'] = array_filter($form_state['values']['tickbox']);
  $count = count($form_state['values']['tickbox']);
  if ($count != 2) {
    form_set_error('tickbox', t('Please select 2 revisions rather than !count', array('!count' => $count)));
  }
}

/**
 * Submit two selected revisions to Diff module.
 * 
 * @param $form
 * @param $form_state
 * @return void
 */
function revisioning_revisions_summary_submit($form, &$form_state) {
  $selected_vids = $form_state['values']['tickbox'];
  $vid1 = key($selected_vids);
  next($selected_vids);
  $vid2 = key($selected_vids);
  drupal_get_messages(); // clear existing msgs
  drupal_set_message(t('Comparing revision #!revision2 against revision #!revision1',
                     array('!revision2' => $vid2, '!revision1' => $vid1)));
  $nid = $form_state['values']['nid'];
  $form_state['redirect'] = "node/$nid/revisions/view/$vid2/$vid1";
}

/**
 * Theme the supplied form.
 * 
 * Uses the following subthemes:
 * o 'table_revisions', falling back to theme.inc/theme_table() if not defined
 * o 'placeholder' (to display current revision status)
 * o 'username'
 * Uses the following style-classes (see revisioning.css)
 * o 'table-revisions'
 * o 'revision-current' and 'published'
 * o 'revision-pending'
 * @param $form
 * @return unknown_type
 */
function theme_revisioning_revisions_summary($form) {

  // Need node info, fortunately node_load() employs a cache so is efficient
  $node = node_load($form['nid']['#value']);

  $css_path = drupal_get_path('module', 'revisioning') .'/revisioning.css';
  drupal_add_css($css_path, 'module', 'all', FALSE); 
  
  // Set up the table rows
  $rows = array();
  $revision_ids = element_children($form['info']);

  $show_diff = count($revision_ids) >= 2 && module_exists('diff');
  $show_taxonomy_terms = module_exists('taxonomy');

  // Set up the table header
  $header = array(t('Revision'));
  if ($show_diff) {
    $header[] = array('data' => drupal_render($form['submit']), 'class' => 'form-submit');
  }
  if ($show_taxonomy_terms) {
    $header[] = t('Term');
  }
  $header[] = t('Status');

  foreach ($revision_ids as $vid) {
    $row = array();
    // Revision info
    $row[] = drupal_render($form['info'][$vid]);
    // Compare checkbox
    if ($show_diff) {
      $row[] = array('data' => drupal_render($form['tickbox'][$vid]));
    }
    // Term
    if ($show_taxonomy_terms) {
      $row[] = drupal_render($form['term'][$vid]);
    }
    // Publication status
    $is_current = ($vid == $node->vid);
    $is_pending = ($vid > $node->vid);
    if ($is_current) {
      $row[] = array('data' => theme('placeholder', $node->status ? t('current revision (published)') : t('current revision (unpublished)')));
    }
    else {
      $row[] = array('data' => $is_pending ? t('pending moderation') : t('old')); 
    }
    $row_style = $is_current ? 'revision-current' : ($is_pending ? 'revision-pending' : NULL);
    if ($is_current && $node->status) {
      $row_style .= ' published';
    }
    $rows[] = array('data' => $row, 'class' => $row_style);
  }
  $attributes = array('class' => 'table-revisions');
  $content = theme(array('table_revisions', 'table'), $header, $rows, $attributes, $caption = NULL);
  $submenu_links = generate_node_links_according_to_permissions($node);
  return theme(array('revisions_summary'), $submenu_links, $content) . drupal_render($form);
}

/**
 * Implement (in your own module) the function below if you want to override
 * the way in which the Revisions table is constructed.
 * If you do, don't forget to register this theme_hook() via <your_module>_theme()
 * in a manner similar to revisioning_theme() in this file.
 * 
 * @param $header
 * @param $rows
 * @return themed HTML, see for instance /includes/theme.inc/theme_table() and
 *         diff.module/theme_diff_table()
 *
 * @ingroup themeable
 * 
function theme_table_revisions($header, $rows) {
}
 */

/**
 * Implementation of <mymodule>_preprocess_hook().
 * Gets called after the node has been fleshed out, but before it is rendered
 * (via node.tpl.php). This gives us the opportunity to insert on the page some 
 * hyperlinks that operate on the content shown, provided the theme is rendered
 * through an ENGINE -- Chameleon, for instance, is NOT, see
 * _theme_content_menu().
 * Earlier versions did this by (ab)using drupal_set_message($submenu_links).
 * Here we prepend the links, themed as a 'revision', to the content. 
 * This ensures the links are visible in all existing templates.
 * 
 * Uses the following subthemes:
 * o revision.tpl.php
 *
 * @param $variables
 * @return void
 */
function revisioning_preprocess_node(&$variables) {
  $args = arg();
  // Only interested in URIs that start with "node/<nid>"
  if ($args[0] != 'node' || !is_numeric($args[1])) {
    return;
  }
  $css_path = drupal_get_path('module', 'revisioning') .'/revisioning.css';
  drupal_add_css($css_path, 'module', 'all', FALSE); // also loads /revisioning-rtl.css
  $submenu_links = generate_revision_links_according_to_permissions($variables['node']);
  $variables['content'] = theme(array('revision'), $submenu_links, $variables['content']);
}

/**
 * Renders the submenu links as part of the content. Use for themes like
 * Chameleon that don't employ a theme engine.
 * 
 * Uses the following subtheme:
 * o revision-submenu.tpl.php
 * 
 * @param $node
 * @return themed HTML
 */

function _theme_content_menu(&$node) {
  $css_path = drupal_get_path('module', 'revisioning') .'/revisioning.css';
  drupal_add_css($css_path, 'module', 'all', FALSE); // also loads /revisioning-rtl.css
  $submenu_links = generate_revision_links_according_to_permissions($node);
  $node->body = theme(array('revision_submenu', 'submenu'), $submenu_links) . $node->body;
}

