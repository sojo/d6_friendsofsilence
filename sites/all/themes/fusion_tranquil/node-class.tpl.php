<?php
// $Id: node.tpl.php 6795 2010-03-16 23:30:49Z jeremy $
?>
<!-- start node.tpl.php -->
<div id="node-<?php print $node->nid; ?>" class="node <?php print $node_classes; ?>">

  <?php if ($page == 1): ?>
  <div class="node-corners rounded-corners"><div class="corner-top-right corner"></div><div class="corner-top-left corner"></div></div><!-- /node-corners -->
  <?php endif; ?>

    <div class="node-inner clearfix">

      <?php if ($title && ($page != 0)): ?>
      <h1 class="title"><?php print $title; ?></h1>
      <?php endif; ?>

	    <?php if ($node_top && !$teaser): ?>
	    <div id="node-top" class="node-top row nested">
	      <div id="node-top-inner" class="node-top-inner inner">
	        <?php print $node_top; ?>
	      </div><!-- /node-top-inner -->
	    </div><!-- /node-top -->
	    <?php endif; ?>

      <?php if ($page == 0): ?>
      <h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
      <?php endif; ?>
      
      <?php 
        //only display cart links if not a member
        global $user;
        $usergroups = array();
        foreach($user->og_groups as $mygroups) {
          $usergroups[] = $mygroups['nid'];
        }
        if (!in_array($node->nid, $usergroups)):
      ?>
        <div id="buy-class-links">
          <?php print $node->content['display_price']['#value'] ?>
          <?php print $node->content['add_to_cart']['#value'] ?>  
        </div><!--/buy-class-links -->
      <?php endif; //end checking if not group member ?>


      <div class="content clearfix">        
        <?php print $node->content['og_mission']['#value'] ?>
      </div>
      

<div class="class-lessons clear">
	<?php print $node->content['classes_node_content_1']['#value'] ?>
</div>

<div id="unpublished-lessons" class="class-lessons clear">
	<?php print $node->content['classes_node_content_2']['#value'] ?>
</div>

    </div><!-- /node-inner -->

  <?php if ($page == 1): ?>
  <div class="node-corners rounded-corners"><div class="corner-bottom-right corner"></div><div class="corner-bottom-left corner"></div></div><!-- /node-corners -->
  <?php endif; ?>

</div>
<!-- /#node-<?php print $node->nid; ?> -->