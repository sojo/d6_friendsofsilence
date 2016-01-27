<?php
// $Id: comment-wrapper.tpl.php 6555 2010-02-24 20:21:15Z chris $

/**
 * @file comment-wrapper.tpl.php
 * Default theme implementation to wrap comments.
 *
 * Available variables:
 * - $content: All comments for a given page. Also contains sorting controls
 *   and comment forms if the site is configured for it.
 *
 * The following variables are provided for contextual information.
 * - $node: Node object the comments are attached to.
 * The constants below the variables show the possible values and should be
 * used for comparison.
 * - $display_mode
 *   - COMMENT_MODE_FLAT_COLLAPSED
 *   - COMMENT_MODE_FLAT_EXPANDED
 *   - COMMENT_MODE_THREADED_COLLAPSED
 *   - COMMENT_MODE_THREADED_EXPANDED
 * - $display_order
 *   - COMMENT_ORDER_NEWEST_FIRST
 *   - COMMENT_ORDER_OLDEST_FIRST
 * - $comment_controls_state
 *   - COMMENT_CONTROLS_ABOVE
 *   - COMMENT_CONTROLS_BELOW
 *   - COMMENT_CONTROLS_ABOVE_BELOW
 *   - COMMENT_CONTROLS_HIDDEN
 *
 * @see template_preprocess_comment_wrapper()
 * @see theme_comment_wrapper()
 */
?>

<?php if ($content) : ?>
<div id="comments" class="block tranquil-border-rounded-corners"> 
  <div class="corners-top rounded-corners"><div class="corner-top-right corner"></div><div class="corner-top-left corner"></div></div>
  <div id="comments-wrapper" class="clearfix">  
    <div class="titlewrapper">
      
      <h2 class="comments-title"><?php print $node->comment_count ?> <?php print t('Comments'); ?></h2>
    </div>
    <div class="comments-content">
      <?php print $content; ?>
    </div>
  </div><!-- /comments-wrapper --> 
  <div class="corners-bottom rounded-corners"><div class="corner-bottom-right corner"></div><div class="corner-bottom-left corner"></div></div>
</div>
<?php endif; ?>