Drupal.behaviors.modulename_subidentifier = function(context) {   
     		 
     	// container start
     	
     		////////////////////////////////////////////////////////////////////
     		  		
     		
     		////////////////////////////////////////////////////////////////////
     	   
            $('#timetracker-mytime-').ajaxForm({
            success: function(result){
            	
					var d = new Date();
	
					var month = d.getMonth()+1;
					var day = d.getDate();
					
					var output = d.getFullYear() + '-' +
						((''+month).length<2 ? '0' : '') + month + '-' +
						((''+day).length<2 ? '0' : '') + day;
						
					var section = result.split('}{');
					
					// send a message to the user
					message(section[0]);
					
					// show any previously hidden tables
					//$('#table-').show();
					$("div:hidden[id*='table-']").show();
					
					// hide the working table 
					// $('#table-' + section[0]).hide();
					
					
					// most recent table
					$('div#most-recent-events table:eq(1) > tbody').prepend(section[1]);
					
					// remove the last rows to keep the list short ( 5 rows ) for most recent
					var rowCount = $("div#most-recent-events > table:eq(1) > tbody > tr").length;

					if(rowCount > 5)
					{
						for (i = rowCount; i > rowCount - 3; i--)
						{
							$('div#most-recent-events table:eq(1) > tbody > tr:eq(' + i +')').remove();
						}
					}
					
					// Add table if no previous records are found for the date
					$("div#most-recent-events").after(section[3]);
					
					// daily table updates				
					$('div#table-' + section[0] + ' table > thead').append(section[1]);
					
					
					
					// using the same html as for the recent list so the id has be changed here
					// to make the show() work
					
					$('div#table-' + section[0] + ' table > thead > #recent-form-row-' + section[4]).attr("id","form-row-" + section[4]);				
			
					$('div#table-' + section[0] + ' table > thead > #recent-' + section[4]).attr("id","row-" + section[4]);
									
					$('div#table-' + section[0] + ' table > thead > #row-'  + section[4] + ' > td > a').unbind("click").removeClass().addClass('update-time').live('click', function() { 
							
							$("div#table-" + section[0] + " table > thead > #form-row-" + section[4]).addClass('temp-form').fadeIn("slow", function () {
									$(this).css('display', 'visible');		
							});
					});
							
					
					
					$('#total-row-' +  section[0] + ' #total-user-hours').text(section[2]);
					
					$('#total-row-' + section[0]).removeClass().addClass('event-added');							

								
									
					
                }
             
            });
            
            ////////////////////////////////////////////////////////////////////
            
            $('#timetracker-myreports-').ajaxForm({
            target:'#output',
            success: function(){
            	            
							$("#my-report-id").val($('#edit-my-projects-id option:selected').val());
						}
            });
            
			$('#update-mini-form').ajaxForm({
			target: this});
						
			$( "#edit-event-date, #edit-start-date, #edit-end-date" ).AnyTime_picker(
				  { format: "%Y-%m-%d",
					formatUtcOffset: "%: (%@)",
					placement: "popup" });
			
			////////////////////////////////////////////////////////////////////
	 
	 		$("a.delete-time").live("click", function(event) {
	 			
	 			event.preventDefault();	
	 			var row = $(this).attr("id").split('-');
	 			var delurl = $(this).attr("href");
								
				$.ajax({
						type: "GET",
						url: delurl,
						dataType: "html",
						cache: false,
						success: function(result){
							
							section = result.split('}{');
							
							$(".row-" + row[1]).fadeOut('slow', function(){
									$(this).remove();							
							});
							
							$("tr:visible[id*='form-row-" + row[1]).remove();
							
							
							$("tr:visible[id*='recent-']" + row[1]).fadeOut('slow', function(){
									$(this).remove();							
							});
							
							$(".recent-" + row[1]).fadeOut('slow', function(){
									$(this).remove();							
							});
							
							$("tr:visible[id*='recent-form-row-" + row[1]).remove();
														
							$('#total-row-' + section[0] + ' #total-user-hours').text(section[1]);
							
							$('#total-row-' + section[0]).removeClass().addClass('event-removed');							

						
							
						}
				});
				
				return false;
				
			});
			
			////////////////////////////////////////////////////////////////////
			
			$("a.update-time").live("click", function(event) {
							
				event.preventDefault();	
				
	 			var row = $(this).attr("id").split('-');
	 			
	 			$("tr:visible[id*='form-row-']").hide();
	 			
	 			$("tr:visible[id*='recent-form-row-']").hide();
	 				 			
	 			/// reset the sect list to the selected value if not changed by submit
	 			$("#update-entry-" + row[1]).each (function() { this.reset(); });
	 			
	 			$("#form-row-" + row[1]).addClass('temp-form').fadeIn("slow", function () {
							$(this).css('display', '');
							
					});
	 			
	 			$("form#update-entry-" + row[1] + " table tbody tr td.indicator div.working-indicator").ajaxStart(function(){
     				
				  $(this).show();
				  
				}).ajaxStop(function(){
					
				  $(this).hide();				  
				  
				}); 
	 			
			});
			
			////////////////////////////////////////////////////////////////////
			
			$("a.update-recent-time").live("click", function(event) {
					
				event.preventDefault();	
				
	 			var row = $(this).attr("id").split('-');			
	 			
	 			$("tr:visible[id*='recent-form-row-']").hide();
	 			
	 			$("tr:visible[id*='form-row-']").hide();
	 			 			
	 			$('input#re-task-' + row[1]).val($('recent-' + row[1]).text());
	 			
	 			// reset the select list to the selected value if not changed by submit
	 			$("#re-update-entry-" + row[1]).each (function() { this.reset(); });
	 			
	 			$("#recent-" + row[1]).next("#recent-form-row-" + row[1]).addClass('temp-form').fadeIn("slow", function () {
							$(this).css('display','');
					});
	 			
	 			$("form#re-update-entry-" + row[1] + " table tbody tr td.indicator div.working-indicator").ajaxStart(function(){
     				
				  $(this).show();
				  
				}).ajaxStop(function(e){
					
				  $(this).hide();
				  
				}); 
	 			
			});
			
			////////////////////////////////////////////////////////////////////
			
			$("div.form-close").live("click", function(event) {
					$("tr:visible[class*='temp-form']").fadeOut("slow", function () {
							$(this).css({display:"none"});
					});
					
					$("tr:visible[class*='info-row-']").fadeOut("slow", function () {
							$(this).css({display:"none"});
					});

			});
			
			////////////////////////////////////////////////////////////////////
			
			$("a.project-info").live("click", function(event) {
					
					event.preventDefault();
					
					var row = $(this).attr("id").split('-');
					
					$("tr:visible[class*='info-row-']").hide();
					
					$('tr.info-row-' + row[2]).fadeIn("slow", function () {
							
							$(this).css('display', '');
							
					});
					
					return false;

			});
			////////////////////////////////////////////////////////////////////
			
			$(".recentUpdate").live("click", function() {
					
					
				var row = $(this).parents().eq(6).attr('id').split('-');
				
				
				$.ajax({
							type: "POST",
							dataType: "html",
							url: "../timetracker/update_mytime/",
							data:{
								id:$('input#re-form-id-' + row[3]).val(),
								task:$('input#re-task-' + row[3]).val(),
								hours:$('input#re-hours-' + row[3]).val(),
								project:$('select#re-projects-' + row[3]).val(),
							},
							cache: false,
							success: function(result){
								
								section = result.split('}{');
								
								// send a message to the user
								if(section[5] != 'undefined')
								{
									message(section[5]);
								}
								else
								{
									message(result);
								}
								
								$('#recent-' + section[0]).fadeOut("slow", function(){

										$(this).replaceWith(section[4]);									
								
										$('#recent-' + section[0]).fadeIn("slow");
								
								});
								
								$('.row-' + section[0]).fadeOut("slow", function(){
										
										$(this).replaceWith(section[1]);
								
										$('.row-' + section[0]).fadeIn("slow");
								
										$('#total-row-' + section[3] + ' #total-user-hours').text(section[2]);
								
										$('#total-row-' + section[3]).removeClass().addClass('event-added');							

								});
								
							}
				});
				
				
				return false;			
				
			});
			
			////////////////////////////////////////////////////////////////////
			
			$("#sendUpdate").live("click",function() {
					
				// show working indicator
				
					
				var row = $(this).parents().eq(6).attr('id').split('-');
				$.ajax({
							type: "POST",
							dataType: "html",
							url: "../timetracker/update_mytime/",
							data:{
								id:$('input#form-id-' + row[2]).val(),
								task:$('input#edit-task-' + row[2]).val(),
								hours:$('input#edit-hours-' + row[2]).val(),
								project:$('select#projects-' + row[2]).val(),
							},
							cache: false,
							success: function(result){
								
								section = result.split('}{');
								
								// send a message to the user
								// send a message to the user
								if(section[5] != 'undefined')
								{
									message(section[5]);
								}
								else
								{
									message(result);
								}
								
								$('.row-' + section[0]).fadeOut("slow", function(){
										

										$(this).replaceWith(section[1]);
								
										$('.row-' + section[0]).fadeIn("slow");
								
										$('#total-row-' + section[3] + ' #total-user-hours').text(section[2]);
								
										$('#total-row-' + section[3]).removeClass().addClass('event-added');
								

								});
								
								$('#recent-' + section[0]).fadeOut("slow", function(){

										$(this).replaceWith(section[4]);									
								
										$('#recent-' + section[0]).fadeIn("slow");
								
								});
								
							}
				});
				
				
				return false;			
				
			});
			
			////////////////////////////////////////////////////////////////////
			
			$('#edit-export').live("click", function(){
				
					$('#timetracker-myreports-csv-').attr("action", '../timetracker/myreport_csv/');
				
					$('#timetracker-myreports-csv-').submit();
				
			});
			
			////////////////////////////////////////////////////////////////////
			function message(html)
			{
				
				$("#message-green").show();
				$("#message-green .message").html(html);
			
				$(".close-green").click(function () {
					$("#message-green").fadeOut("slow");
				});
				
				setTimeout(function(){
					$("#message-green").fadeOut("slow");
				},5000)
				
			}
						

		  // container end
}
    
