<?php echo (isset($links) ? $links : ''); ?><br />
<h1 id="page-title" class="title"><?php echo (isset($title) ? $title : ''); ?></h1><br />
<div id="message-green" style="display:none">
<div style="text-align: right;"><a href="#"><span class="close-green">Close X</span></a></div>
<div class="message"></div>
</div>
<?php echo (isset($content) ? $content : ''); ?>

