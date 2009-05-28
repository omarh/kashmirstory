<?php
// $Id: module_grants_theme.inc,v 1.2 2009/04/20 00:11:22 rdeboer Exp $
/**
 * @file
 *  Include file for module_grants.module, dealing with all theming aspects.
 */

/**
 * Implementation of hook_theme().
 * 
 * Register the theme_hooks() available in this module, with their arguments
 * and default values.
 */
function module_grants_theme() {
  return array(
    'nodes_summary' => array(
       'arguments' => array('nodes' => NULL) // theme_nodes_summary($nodes)
    )
  );
}

/**
 * Theme the passed-in nodes as a table.
 * 
 * Uses the following subthemes:
 * o 'table_nodes', falling back to theme.inc/theme_table() if not defined
 * o 'username', i.e. theme.inc/theme_username()
 *
 * @param $nodes
 *   Array of nodes to display.
 * @return
 *   Themed table HTML or a paragraph saying 'No content found.' if the supplied
 *   array is empty.
 *   
 * @ingroup themeable
 */
function theme_nodes_summary($nodes) {
  $css_path = drupal_get_path('module', 'module_grants') .'/module_grants.css';
  drupal_add_css($css_path, 'module', 'all', FALSE); // also loads /module_grants-rtl.css
  if (!empty($nodes)) {
    $header = array(t('Title'), t('Type'), t('Last updated'), t('By'), t('Published?'));
    $show_taxonomy_terms = module_exists('taxonomy');
    $show_workflow_state = module_exists('workflow');
    if ($show_taxonomy_terms) {
      $header[] = t('Term');
    }
    if ($show_workflow_state) {
      $header[] = t('Workflow state');
    }
    $rows = array();
    $page_link = user_access('view revisions') ? 'revisions' : 'view';
    foreach ($nodes as $node) {
      $row = array(
        l($node->title, "node/$node->nid/$page_link"),
        check_plain(node_get_types('name', $node)),
        format_date($node->timestamp),
        theme('username', user_load(array('uid' => $node->uid))),
        $node->status ? t('Yes') : t('No')
      );
      if ($show_taxonomy_terms) {
        $row[] = empty($node->term) ? '' : check_plain($node->term);
      }
      if ($show_workflow_state) {
        $row[] = empty($node->state) ? t('No state') : check_plain($node->state);
      }
      $rows[] = $row;
    }
    $attributes = array('class' => 'table-nodes');
    return theme(array('table_nodes', 'table'), $header, $rows, $attributes, $caption = NULL);
  }
  return '<p>'. t('No content found.') .'</p>';
}

/**
 * Implement (in your own module) the function below if you want to override
 * the default way in which the nodes summary table is displayed.
 * If you do, don't forget to register this theme_hook() via <your_module>_theme()
 * in a manner similar to module_grants_theme() in this file.
 * 
 * @param $header
 * @param $rows
 * @return themed HTML, see for instance /includes/theme.inc/theme_table()
 *
 * @ingroup themeable
 * 
function theme_table_nodes($header, $rows) {
}
 */